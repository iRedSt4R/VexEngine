#pragma once
#include "../../Common/VexRenderCommon.h"
#include "../D3D12/DX12IndexedVertexBuffer.h"
#include "../D3D12/DX12ConstantBuffer.h"
#include "../Camera/FPSCamera.h"
#include "../Textures/3DTexture.h"

class Skybox
{
public:
	Skybox(ID3D12Device* device) 
		: m_device(device) 
	{
		m_indxedVertexBuffer = new DX12IndexedVertexBuffer(device);
		m_cubeMapCB = new ConstantBuffer<CBCubeMapData>(device);
		m_cubemapSRV = new TextureCube(device);
	};

	~Skybox() 
	{
		delete m_cubeMapCB;
		delete m_indxedVertexBuffer;
	};

	void CreateSkyboxSphere(ID3D12GraphicsCommandList* cmdList, std::wstring pathToDDSCubemap, int latLines, int longLines);

	// skybox will follow camera, so camera is always as the centre of skybox
	void Update(FPSCamera* skyboxCenterCamera);

	// bind constant buffer with WVP for cubemape, bind vertex and index buffer and draw
	__forceinline void Draw(ID3D12GraphicsCommandList* cmdList, UINT cbRootBindIndex) 
	{
		BindIndexedVertexBuffer(cmdList);  
		BindConstantBuffer(cmdList, cbRootBindIndex);
		cmdList->DrawIndexedInstanced(m_numSphereFaces * 3, 1, 0, 0, 0);
	}

	__forceinline ConstantBuffer<CBCubeMapData>* GetConstantBuffer() { return m_cubeMapCB; }
	__forceinline DX12IndexedVertexBuffer* GetIndexedVertexBuffer() { return m_indxedVertexBuffer; }
	__forceinline void BindIndexedVertexBuffer(ID3D12GraphicsCommandList* cmdList) { m_indxedVertexBuffer->Set(cmdList); }
	__forceinline void BindConstantBuffer(ID3D12GraphicsCommandList* cmdList, UINT cbRootBindIndex) { m_cubeMapCB->SetAsInlineRootDescriptor(cmdList, cbRootBindIndex); }
	__forceinline TextureCube* GetCubemapTexture() { return m_cubemapSRV; }
private:

	ID3D12Device* m_device = nullptr;
	DX12IndexedVertexBuffer* m_indxedVertexBuffer = nullptr;

	int m_numSphereVertices = 0;
	int m_numSphereFaces = 0;

	ConstantBuffer<CBCubeMapData>* m_cubeMapCB = nullptr;
	TextureCube* m_cubemapSRV = nullptr;
	
	uint32_t skyboxHeight = 0;
	uint32_t skyboxWidth = 0;

	//DEBUG
	XMMATRIX Rotationx;
	XMMATRIX Rotationy;
	XMMATRIX Rotationz;
	XMMATRIX sphereWorld;
	XMMATRIX Scale;
	XMMATRIX Translation;
	XMMATRIX WVP;

};

class SkyboxSerializer
{
	static void Serialize(Skybox* pSkybox, std::string pathToSerialize);
	static Skybox* Deserialize(std::string pathToSkyboxBlob);
};