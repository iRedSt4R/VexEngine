#include "Mesh.h"

//#define SCALE 0.005f
#define SCALE 0.5f
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
void SimpleMesh::LoadMesh(const aiScene* scene, aiNode* meshNode, int meshIndex, std::string meshFolder)
{
	const aiMesh* aMesh = meshNode == nullptr ? scene->mMeshes[meshIndex] : scene->mMeshes[meshNode->mMeshes[0]];
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
	//if (meshNode)
	//	m_meshCB->CPUData().meshWorldMatrix = DirectX::XMFLOAT4X4((const float*)(&(meshNode->mTransformation)));
	//else
		//XMStoreFloat4x4(&m_meshCB->CPUData().meshWorldMatrix, DirectX::XMMatrixIdentity());
		//m_meshCB->CPUData().meshWorldMatrix = DirectX::XMMatrixIdentity();

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
#if VEX_COOKER
		m_albedoSRV = Texture2D::LoadFromFile(utf8toUtf16(pathCombined));
		if (m_albedoSRV != nullptr)
		{
			m_materialHeader.bHaveAlbedoTexture = 1;
			m_materialHeader.albedoTextureByteSize = m_albedoSRV->GetBufferSize();
			m_materialHeader.albedoTextureOffset = 0;
		}
#else
		m_albedoSRV->CreateFromFile(m_CmdList, utf8toUtf16(pathCombined));

		// set cb data as well for this texture
		m_meshCB->CPUData().albedoIndexInHeap = m_albedoSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
		m_meshCB->CPUData().bHaveAlbedoTex = true;
#endif
	}
	else
	{
		aiMaterial* material = scene->mMaterials[aMesh->mMaterialIndex];

		aiColor3D color(0.f, 0.f, 0.f);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		m_meshCB->CPUData().color.x = color.r;
		m_meshCB->CPUData().color.y = color.g;
		m_meshCB->CPUData().color.z = color.b;
	}

	// normal map
	if (meshMaterial->GetTexture(aiTextureType_NORMALS, texIndex, &path) == AI_SUCCESS)
	{
		std::string texturePath(path.data);
		std::string pathCombined(assetTexturePath + texturePath);
		errorPath = pathCombined;
#if VEX_COOKER
		m_normalSRV = Texture2D::LoadFromFile(utf8toUtf16(pathCombined));
		if (m_normalSRV != nullptr)
		{
			m_materialHeader.bHaveNormalTexture = 1;
			m_materialHeader.normalTextureByteSize = m_normalSRV->GetBufferSize();
			m_materialHeader.normalTextureOffset = m_materialHeader.albedoTextureByteSize;
		}
#else
		m_normalSRV->CreateFromFile(m_CmdList, utf8toUtf16(pathCombined), false);

		m_meshCB->CPUData().normalIndexInHeap = m_normalSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
		m_meshCB->CPUData().bHaveNormalTex = true;
#endif
	}
	aiString fileMetallic;
	// roughness and metallic combined
	if (meshMaterial->GetTexture(aiTextureType_UNKNOWN, texIndex, &path) == AI_SUCCESS)
	{
		std::string texturePath(path.data);
		std::string pathCombined(assetTexturePath + texturePath);
		errorPath = pathCombined;
#if VEX_COOKER
		m_roughnessMetallicSRV = Texture2D::LoadFromFile(utf8toUtf16(pathCombined));
		if (m_roughnessMetallicSRV != nullptr)
		{
			m_materialHeader.bHaveRoughnessTexture = 1;
			m_materialHeader.bHaveMetallicTexture = 1;
			m_materialHeader.roughnessTextureByteSize = m_roughnessMetallicSRV->GetBufferSize();
			m_materialHeader.roughnessTextureOffset = m_materialHeader.albedoTextureByteSize + m_materialHeader.normalTextureByteSize;
			m_materialHeader.metallicTextureByteSize = m_roughnessMetallicSRV->GetBufferSize();
			m_materialHeader.metallicTextureOffset = m_materialHeader.albedoTextureByteSize + m_materialHeader.normalTextureByteSize;
		}
#else
		m_roughnessMetallicSRV->CreateFromFile(m_CmdList, utf8toUtf16(pathCombined), false);

		m_meshCB->CPUData().roughnessIndexInHeap = m_roughnessMetallicSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
		m_meshCB->CPUData().metallicIndexInHeap = m_roughnessMetallicSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
		m_meshCB->CPUData().bHaveRoughnessTex = true;
		m_meshCB->CPUData().bHaveMetallicTex = true;
#endif
	}
	else
	{
		aiMaterial* material = scene->mMaterials[aMesh->mMaterialIndex];

		//aiColor3D color(0.f, 0.f, 0.f);
		float roughnessFactor = 1.f;
		material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughnessFactor);
		m_meshCB->CPUData().roughness = roughnessFactor;

		float metallicFactor = 0.f;
		material->Get(AI_MATKEY_METALLIC_FACTOR, metallicFactor);
		m_meshCB->CPUData().metalness = metallicFactor;


	}
#if !VEX_COOKER
	m_IndexedVertexBuffer->Create(m_CmdList, m_VertexCount * 4 * VERTEX_ATTRIBUTE_COUNT, VERTEX_ATTRIBUTE_COUNT * 4, m_IndexCount, m_Vertices, m_Indicies);
#endif

#if VEX_COOKER
	m_materialHeader.binaryByteSize = m_materialHeader.albedoTextureByteSize + m_materialHeader.normalTextureByteSize + m_materialHeader.roughnessTextureByteSize;
#endif

	m_meshHeader.textureContentType = 0;
	m_meshHeader.vertexByteSize = aMesh->mNumVertices * VERTEX_ATTRIBUTE_COUNT * sizeof(float);
	m_meshHeader.indexByteSize = aMesh->mNumFaces * 3 * sizeof(uint32_t);
	m_meshHeader.DDSByteSize = 0;

	m_staticMeshHeader.meshContentType = 0;
	m_staticMeshHeader.vertexByteSize = aMesh->mNumVertices * VERTEX_ATTRIBUTE_COUNT * sizeof(float);
	m_staticMeshHeader.vertexByteOffset = 0;
	m_staticMeshHeader.indexByteSize = aMesh->mNumFaces * 3 * sizeof(uint32_t);
	m_staticMeshHeader.indexByteOffset = m_staticMeshHeader.vertexByteSize;
	m_staticMeshHeader.fullByteSize = m_staticMeshHeader.vertexByteSize + m_staticMeshHeader.indexByteSize;
}

void SimpleMesh::LoadBinaryMesh(char* binaryData, StaticMeshHeader& meshHeader, PBRMaterialHeader& materialHeader, uint32_t blobOffset)
{
#if !VEX_COOKER
	m_VertexCount = meshHeader.vertexByteSize / 4 * VERTEX_ATTRIBUTE_COUNT;
	m_IndexCount = meshHeader.indexByteSize / 4;

	char* vertexBufferOffset = binaryData + blobOffset;
	char* indexBufferOffset = binaryData + blobOffset + meshHeader.vertexByteSize;
	char* albedoOffset = binaryData + blobOffset + meshHeader.fullByteSize;
	char* normalOffset = albedoOffset + materialHeader.albedoTextureByteSize;
	char* roughnessOffset = normalOffset + materialHeader.normalTextureByteSize;

	// index and vertex buffer
	m_IndexedVertexBuffer->Create(m_CmdList, meshHeader.vertexByteSize, VERTEX_ATTRIBUTE_COUNT * 4, meshHeader.indexByteSize / 4, (void*)vertexBufferOffset, (void*)indexBufferOffset);

	// albedo texture
	m_albedoSRV = new Texture2D(m_Device);
	if(materialHeader.albedoTextureByteSize > 0)
		m_albedoSRV->CreateFromBinary(m_CmdList, albedoOffset, materialHeader.albedoTextureByteSize, true);

	// normal texture
	m_normalSRV = new Texture2D(m_Device);
	if(materialHeader.normalTextureByteSize > 0)
		m_normalSRV->CreateFromBinary(m_CmdList, normalOffset, materialHeader.normalTextureByteSize, true);
	else
		m_normalSRV->CreateFromBinary(m_CmdList, albedoOffset, materialHeader.albedoTextureByteSize, true);

	// roughness texture
	m_roughnessMetallicSRV = new Texture2D(m_Device);
	if (materialHeader.roughnessTextureByteSize > 0)
		m_roughnessMetallicSRV->CreateFromBinary(m_CmdList, roughnessOffset, materialHeader.roughnessTextureByteSize, true);
	else
		m_roughnessMetallicSRV->CreateFromBinary(m_CmdList, albedoOffset, materialHeader.albedoTextureByteSize, true);


	m_meshCB->CPUData().albedoIndexInHeap = m_albedoSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
	m_meshCB->CPUData().bHaveAlbedoTex = true;
	m_meshCB->CPUData().normalIndexInHeap = m_normalSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
	m_meshCB->CPUData().bHaveNormalTex = true;
	m_meshCB->CPUData().roughnessIndexInHeap = m_roughnessMetallicSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
	m_meshCB->CPUData().metallicIndexInHeap = m_roughnessMetallicSRV->GetDX12Resource()->GetSRVIndexInsideHeap();
	m_meshCB->CPUData().bHaveRoughnessTex = true;
	m_meshCB->CPUData().bHaveMetallicTex = true;
	m_meshCB->SendConstantDataToGPU();
#endif
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
	if (1)
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
#if VEX_COOKER
	std::ofstream meshBinary(pathToSerialize, std::ios::binary);	
	meshBinary.write((char*)&m_staticMeshHeader, sizeof(StaticMeshHeader));
	meshBinary.write((char*)m_Vertices, m_meshHeader.vertexByteSize);
	meshBinary.write((char*)m_Indicies, m_meshHeader.indexByteSize);
#endif
}

void SimpleMesh::SerializeHeader(std::ofstream& stream)
{
#if VEX_COOKER
	stream.write((char*)&m_staticMeshHeader, sizeof(StaticMeshHeader));
	stream.write((char*)&m_materialHeader, sizeof(PBRMaterialHeader));
#endif
}

void SimpleMesh::SerializeBinary(std::ofstream& stream)
{
#if VEX_COOKER
	stream.write((char*)m_Vertices, m_meshHeader.vertexByteSize);
	stream.write((char*)m_Indicies, m_meshHeader.indexByteSize);
	if(m_materialHeader.bHaveAlbedoTexture)
		stream.write((char*)m_albedoSRV->GetBufferPointer(), m_albedoSRV->GetBufferSize());
	if (m_materialHeader.bHaveNormalTexture)
		stream.write((char*)m_normalSRV->GetBufferPointer(), m_normalSRV->GetBufferSize());
	if (m_materialHeader.bHaveRoughnessTexture)
		stream.write((char*)m_roughnessMetallicSRV->GetBufferPointer(), m_roughnessMetallicSRV->GetBufferSize());
#endif
}

// ----------------------- Mesh ----------------------- //
void Mesh::LoadMesh(std::string filePath, std::string meshFolder)
{
	std::string completePath = "Assets/" + filePath;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(completePath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs);

	//for (int meshID = 0; meshID < scene->mRootNode->mNumChildren; meshID++)
	for (int meshID = 0; meshID < scene->mNumMeshes; meshID++)
	{
#if VEX_COOKER
		SimpleMesh* simpleMesh = new SimpleMesh();
#else
		SimpleMesh* simpleMesh = new SimpleMesh(m_Device, m_CmdList);
#endif
		//simpleMesh->LoadMesh(scene, scene->mRootNode->mChildren[meshID], meshID, meshFolder);
		simpleMesh->LoadMesh(scene, nullptr, meshID, meshFolder);
		m_Meshes.push_back(simpleMesh);
	}
}

void Mesh::LoadBinaryMesh(std::string filePathToBlob)
{
#if !VEX_COOKER
	std::ifstream meshBinary(filePathToBlob, std::ios::in | std::ios::binary);
	auto fileSize = std::filesystem::file_size(filePathToBlob);
	char* data = new char[fileSize];
	meshBinary.read(data, fileSize);

	MeshHeader* meshHeader = reinterpret_cast<MeshHeader*>(data);
	m_Meshes.resize(meshHeader->meshCount);
	// binary data start after all headers
	uint32_t binaryDataStartOffset = sizeof(MeshHeader) + (meshHeader->meshCount * (sizeof(StaticMeshHeader) + sizeof(PBRMaterialHeader)));

	char* currPtr = data + sizeof(MeshHeader);
	for (uint32_t id = 0; id < meshHeader->meshCount; id++)
	{
		StaticMeshHeader* staticMeshHeader = reinterpret_cast<StaticMeshHeader*>(currPtr);
		currPtr += sizeof(StaticMeshHeader);
		PBRMaterialHeader* materialHeader = reinterpret_cast<PBRMaterialHeader*>(currPtr);
		currPtr += sizeof(PBRMaterialHeader);

		m_Meshes[id] = new SimpleMesh(m_Device, m_CmdList);
		m_Meshes[id]->LoadBinaryMesh(data, *staticMeshHeader, *materialHeader, binaryDataStartOffset);

		binaryDataStartOffset += staticMeshHeader->fullByteSize + materialHeader->binaryByteSize;
	}
#endif
}

void Mesh::DrawMesh(bool bShadow)
{
	for (auto& mesh : m_Meshes)
	{
		mesh->DrawMesh(bShadow);
	}
}

void Mesh::Serialize(std::filesystem::path pathToSerialize)
{
	std::ofstream meshBinary(pathToSerialize, std::ios::binary);
	
	m_meshHeader.meshCount = m_Meshes.size();
	meshBinary.write((char*)&m_meshHeader, sizeof(MeshHeader));

	for (auto& mesh : m_Meshes)
	{
		mesh->SerializeHeader(meshBinary);
	}

	BYTE offset = 0;
	for (auto& mesh : m_Meshes)
	{
		mesh->SerializeBinary(meshBinary);
		offset += mesh->m_staticMeshHeader.fullByteSize;
	}
	//meshBinary.write((char*)m_Vertices, m_meshHeader.vertexByteSize);
	//meshBinary.write((char*)m_Indicies, m_meshHeader.indexByteSize);
}

void Mesh::Deserialize(std::filesystem::path pathToDeserialize)
{
}
