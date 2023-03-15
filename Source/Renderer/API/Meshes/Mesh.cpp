#include "Mesh.h"

#define SCALE 0.005f
#define VERTEX_ATTRIBUTE_COUNT 14

std::wstring utf8toUtf16(const std::string& str)
{
	if (str.empty())
		return std::wstring();

	size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), NULL, 0);
	if (charsNeeded == 0)
		throw std::runtime_error("Failed converting UTF-8 string to UTF-16");

	std::vector<wchar_t> buffer(charsNeeded);
	int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
		str.data(), (int)str.size(), &buffer[0], buffer.size());
	if (charsConverted == 0)
		throw std::runtime_error("Failed converting UTF-8 string to UTF-16");

	return std::wstring(&buffer[0], charsConverted);
}

// ----------------------- Simple Mesh ----------------------- //
void SimpleMesh::LoadMesh(const aiScene* scene, int meshIndex, std::string meshFolder)
{
	aiMesh* aMesh = scene->mMeshes[meshIndex];
	m_faceCount = aMesh->mNumFaces;
	m_VertexCount = aMesh->mNumVertices;
	m_IndexCount = m_faceCount * 3;

	// for every vertex inside mesh
	m_Vertices = new float[aMesh->mNumVertices * VERTEX_ATTRIBUTE_COUNT];
	for (unsigned int vertexID = 0; vertexID < aMesh->mNumVertices; vertexID++)
	{
		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 0] = aMesh->mVertices[vertexID].x * SCALE;
		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 1] = aMesh->mVertices[vertexID].y * SCALE;
		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 2] = aMesh->mVertices[vertexID].z * SCALE;

		// Normal
		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 3] = aMesh->mNormals[vertexID].x;
		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 4] = aMesh->mNormals[vertexID].y;
		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 5] = aMesh->mNormals[vertexID].z;

		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 6] = aMesh->mTextureCoords[0][vertexID].x;
		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 7] = aMesh->mTextureCoords[0][vertexID].y;

		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 8] = aMesh->mTangents[vertexID].x;
		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 9] = aMesh->mTangents[vertexID].y;
		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 10] = aMesh->mTangents[vertexID].z;

		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 11] = aMesh->mBitangents[vertexID].x;
		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 12] = aMesh->mBitangents[vertexID].y;
		m_Vertices[vertexID * VERTEX_ATTRIBUTE_COUNT + 13] = aMesh->mBitangents[vertexID].z;
	}

	// Load indices from faces
	m_Indicies = new uint32_t[aMesh->mNumFaces * 3];
	for (unsigned int faceID = 0; faceID < aMesh->mNumFaces; faceID++)
	{
		const aiFace& aFace = aMesh->mFaces[faceID];
		m_Indicies[faceID * 3 + 0] = aFace.mIndices[0];
		m_Indicies[faceID * 3 + 1] = aFace.mIndices[1];
		m_Indicies[faceID * 3 + 2] = aFace.mIndices[2];
	}

	// Load textures from material
	aiMaterial* meshMaterial = scene->mMaterials[aMesh->mMaterialIndex];
	int texIndex = 0;
	aiString path;
	std::string assetTexturePath = "Assets/" + meshFolder;
	std::string errorPath = assetTexturePath + "textures/INT_Grid_Rear_baseColor.png";
	// albedo/diffuse
	if (meshMaterial->GetTexture(aiTextureType_DIFFUSE, texIndex, &path) == AI_SUCCESS)
	{
		std::string texturePath(path.data);
		std::string pathCombined(assetTexturePath + texturePath);
		errorPath = pathCombined;
		m_albedoSRV->CreateFromFile(m_CmdList, utf8toUtf16(pathCombined));

		// set cb data as well for this texture
		m_meshCB->CPUData().albedoIndexInHeap = m_albedoSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
		m_meshCB->CPUData().bHaveAlbedoTex = true;
	}

	// normal map
	if (meshMaterial->GetTexture(aiTextureType_NORMALS, texIndex, &path) == AI_SUCCESS)
	{
		std::string texturePath(path.data);
		std::string pathCombined(assetTexturePath + texturePath);
		errorPath = pathCombined;
		m_normalSRV->CreateFromFile(m_CmdList, utf8toUtf16(pathCombined), false);

		m_meshCB->CPUData().normalIndexInHeap = m_normalSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
		m_meshCB->CPUData().bHaveNormalTex = true;
	}
	aiString fileMetallic;
	// roughness and metallic combined
	if (meshMaterial->GetTexture(aiTextureType_UNKNOWN, texIndex, &path) == AI_SUCCESS)
	{
		std::string texturePath(path.data);
		std::string pathCombined(assetTexturePath + texturePath);
		errorPath = pathCombined;
		m_roughnessMetallicSRV->CreateFromFile(m_CmdList, utf8toUtf16(pathCombined), false);

		m_meshCB->CPUData().roughnessIndexInHeap = m_roughnessMetallicSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
		m_meshCB->CPUData().metallicIndexInHeap = m_roughnessMetallicSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
		m_meshCB->CPUData().bHaveRoughnessTex = true;
		m_meshCB->CPUData().bHaveMetallicTex = true;
	}
	m_IndexedVertexBuffer->Create(m_CmdList, m_VertexCount * 4 * VERTEX_ATTRIBUTE_COUNT, VERTEX_ATTRIBUTE_COUNT * 4, m_IndexCount, m_Vertices, m_Indicies);

	m_meshHeader.textureContentType = 0;
	m_meshHeader.vertexByteSize = aMesh->mNumVertices * VERTEX_ATTRIBUTE_COUNT * sizeof(float);
	m_meshHeader.indexByteSize = aMesh->mNumFaces * 3 * sizeof(uint32_t);
	m_meshHeader.DDSByteSize = 0;

	//Serialize(std::string("TEST") + std::to_string(meshIndex) + ".vexmesh");

}

void SimpleMesh::Deserialize(std::filesystem::path blobPath)
{
	std::ifstream meshBinary(blobPath, std::ios::in | std::ios::binary);
	char* data = nullptr;
	auto fileSize = std::filesystem::file_size(blobPath);
	meshBinary.read(data, fileSize);
}

void SimpleMesh::DrawMesh(bool bShadow)
{
	m_meshCB->SendConstantDataToGPU();

	m_IndexedVertexBuffer->Set(m_CmdList);
	if (!bShadow)
	{
		m_meshCB->SetAsInlineRootDescriptor(m_CmdList, 1);
	}

	if(m_IndexedVertexBuffer->OnlyVertexBuffer())
		m_CmdList->DrawInstanced(m_VertexCount, 1, 0, 0);
	else
		m_CmdList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
}

void SimpleMesh::Serialize(std::filesystem::path pathToSerialize)
{
	std::ofstream meshBinary(pathToSerialize, std::ios::binary);	
	meshBinary.write((char*)&m_meshHeader, sizeof(EngineMeshHeader));
	meshBinary.write((char*)m_Vertices, m_meshHeader.vertexByteSize);
	meshBinary.write((char*)m_Indicies, m_meshHeader.indexByteSize);
}

// ----------------------- Mesh ----------------------- //
void Mesh::LoadMesh(std::string filePath, std::string meshFolder)
{
	std::string completePath = "Assets/" + filePath;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(completePath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

	for (int meshID = 0; meshID < scene->mNumMeshes; meshID++)
	{
		SimpleMesh* simpleMesh = new SimpleMesh(m_Device, m_CmdList);
		simpleMesh->LoadMesh(scene, meshID, meshFolder);
		m_Meshes.push_back(simpleMesh);
	}
}

void Mesh::DrawMesh(bool bShadow)
{
	for (auto& mesh : m_Meshes)
	{
		mesh->DrawMesh(bShadow);
	}
}
