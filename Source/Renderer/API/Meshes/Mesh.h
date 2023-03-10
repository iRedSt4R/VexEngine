#pragma once


#include "../../Common/VexRenderCommon.h"
#include "../Textures/2DTexture.h"
#include "../D3D12/DX12IndexedVertexBuffer.h"
#include "../D3D12/DX12ConstantBuffer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


// memory layout : EngineMeshHeader | vertexBufferData | indexBufferData | ddsTextureData
#pragma pack(push, 1)
struct EngineMeshHeader
{
	uint8_t textureContentType = 0; // describe if and how many dds textures it contains
	uint32_t vertexByteSize = 0;
	uint32_t indexByteSize = 0;
	uint32_t DDSByteSize = 0;
};
#pragma pack(pop)

class SimpleMesh
{
public:
	SimpleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) 
	{
		m_Device = device; 
		m_CmdList = cmdList;
		m_IndexedVertexBuffer = new DX12IndexedVertexBuffer(device);
		m_AlbedoSRV = new Texture2D(device);
		m_meshCB = new ConstantBuffer<CBStaticMeshData>(device);
	}
	~SimpleMesh() {};

	void LoadMesh(const aiScene* scene, int meshIndex, std::string meshFolder);
	void DrawMesh(bool bShadow = false);
	void Serialize(std::filesystem::path pathToSerialize);
	void Deserialize(std::filesystem::path blobPath);

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

	Texture2D* m_AlbedoSRV = nullptr;

	// header used for serialization;
	EngineMeshHeader m_meshHeader;
	//ShaderResource* m_MetalnessSRV;
	ConstantBuffer<CBStaticMeshData>* m_meshCB = nullptr;
};

class Mesh
{
	friend class AccelerationStructure;

public:
	Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) 
	{ 
		m_Device = device;
		m_CmdList = cmdList; 
	}
	~Mesh() {};

	void LoadMesh(std::string filePath, std::string meshFolder);
	void DrawMesh(bool bShadow = false);

private:
	std::vector<SimpleMesh*> m_Meshes;

	// D3D12 
	ID3D12Device* m_Device = nullptr;
	ID3D12GraphicsCommandList* m_CmdList = nullptr;
};