#pragma once


#include "../../Common/VexRenderCommon.h"
#include "../Textures/2DTexture.h"
#include "../D3D12/DX12IndexedVertexBuffer.h"
#include "../D3D12/DX12ConstantBuffer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class SimpleMesh
{
public:
	SimpleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) 
	{
		m_Device = device; 
		m_CmdList = cmdList;
		m_IndexedVertexBuffer = new DX12IndexedVertexBuffer(device);
		m_AlbedoSRV = new Texture2D(device);
		m_modelCB = new ConstantBuffer<CBSceneModel>(device);
	}
	~SimpleMesh() {};

	void LoadMesh(const aiScene* scene, int meshIndex, std::string meshFolder);
	void DrawMesh();

private:
	float* m_Vertices;
	float* m_VerticesOnly;
	int m_VertexCount;
	uint32_t* m_Indicies;
	int m_IndexCount;
	int m_faceCount;
	
	// D3D12 
	ID3D12Device* m_Device;
	ID3D12GraphicsCommandList* m_CmdList;
	DX12IndexedVertexBuffer* m_IndexedVertexBuffer;

	Texture2D* m_AlbedoSRV;
	ConstantBuffer<CBSceneModel>* m_modelCB = nullptr;
	//ShaderResource* m_MetalnessSRV;
};

class Mesh
{
	friend class AccelerationStructure;

public:
	Mesh(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) { m_Device = device; m_CmdList = cmdList; }
	~Mesh() {};

	void LoadMesh(std::string filePath, std::string meshFolder);
	void DrawMesh();

private:
	std::vector<SimpleMesh*> m_Meshes;

	// D3D12 
	ID3D12Device* m_Device;
	ID3D12GraphicsCommandList* m_CmdList;
};