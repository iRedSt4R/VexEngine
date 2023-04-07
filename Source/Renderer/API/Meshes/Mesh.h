#pragma once


#include "../../Common/VexRenderCommon.h"
#include "../Textures/2DTexture.h"
#include "../D3D12/DX12IndexedVertexBuffer.h"
#include "../D3D12/DX12ConstantBuffer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Material.h"


// memory layout : EngineMeshHeader | vertexBufferData | indexBufferData | ddsTextureData
#pragma pack(push, 1)
struct EngineMeshHeader
{
	uint8_t textureContentType = 0; // describe if and how many dds textures it contains
	uint32_t vertexByteSize = 0;
	uint32_t indexByteSize = 0;
	uint32_t DDSByteSize = 0;
};

struct StaticMeshHeader
{
	uint8_t meshContentType = 0; // mesh type (for eg. which vertex layout is present)
	uint8_t bHaveIndexBuffer = 0;
	uint32_t vertexByteSize = 0;
	uint32_t vertexByteOffset = 0; // offset from binary data (without header)
	uint32_t indexByteSize = 0;
	uint32_t indexByteOffset = 0; // offset from binary data (without header)
	uint32_t fullByteSize = 0; // full size (binary data)
};

struct MeshHeader
{
	uint32_t meshCount = 0;
};
#pragma pack(pop)

class SimpleMesh
{
	friend class Mesh;

public:
#if VEX_COOKER
	SimpleMesh() {};
#else
	SimpleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) 
	{
		m_Device = device; 
		m_CmdList = cmdList;
		m_IndexedVertexBuffer = new DX12IndexedVertexBuffer(device);
		m_albedoSRV = new Texture2D(device);
		m_normalSRV = new Texture2D(device);
		m_roughnessMetallicSRV = new Texture2D(device);
		m_meshCB = new ConstantBuffer<CBStaticMeshData>(device);
	}
#endif
	~SimpleMesh() {};

	void LoadMesh(const aiScene* scene, int meshIndex, std::string meshFolder);
	void LoadBinaryMesh(char* binaryData, StaticMeshHeader& meshHeader, PBRMaterialHeader& materialHeader, uint32_t blobOffset);
	void DrawMesh(bool bShadow = false);
	void Serialize(std::filesystem::path pathToSerialize);
	void SerializeHeader(std::ofstream& stream);
	void SerializeBinary(std::ofstream& stream);
	void Deserialize(std::filesystem::path blobPath);
	void Deserialize(StaticMeshHeader* meshHeader, BYTE* binaryData);

private:
	float* m_Vertices = nullptr;
	float* m_VerticesOnly = nullptr;
	int m_VertexCount = 0;
	uint32_t* m_Indicies = nullptr;
	int m_IndexCount = 0;
	int m_faceCount = 0;
	
	// D3D12 
	ID3D12Device* m_Device = nullptr;
	ID3D12GraphicsCommandList* m_CmdList = nullptr;
	DX12IndexedVertexBuffer* m_IndexedVertexBuffer = nullptr;

#if !VEX_COOKER
	Texture2D* m_albedoSRV = nullptr;
	Texture2D* m_normalSRV = nullptr;
	Texture2D* m_roughnessMetallicSRV = nullptr;
#else
	DirectX::Blob* m_albedoSRV = nullptr;
	DirectX::Blob* m_normalSRV = nullptr;
	DirectX::Blob* m_roughnessMetallicSRV = nullptr;
#endif

	// header used for serialization;
	EngineMeshHeader m_meshHeader;
	StaticMeshHeader m_staticMeshHeader;
	PBRMaterialHeader m_materialHeader;
	//ShaderResource* m_MetalnessSRV;
	ConstantBuffer<CBStaticMeshData>* m_meshCB = nullptr;
};

class Mesh
{
	friend class AccelerationStructure;

public:
#if VEX_COOKER
	Mesh() {};
#else
	Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) 
	{ 
		m_Device = device;
		m_CmdList = cmdList; 
	}
#endif

	~Mesh() {};

	void LoadMesh(std::string filePath, std::string meshFolder);
	void LoadBinaryMesh(std::string filePathToBlob);
	void DrawMesh(bool bShadow = false);

	void Serialize(std::filesystem::path pathToSerialize);
	void Deserialize(std::filesystem::path pathToDeserialize);

private:
	std::vector<SimpleMesh*> m_Meshes;

	// D3D12 
	ID3D12Device* m_Device = nullptr;
	ID3D12GraphicsCommandList* m_CmdList = nullptr;
	MeshHeader m_meshHeader;
};