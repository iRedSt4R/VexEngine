#pragma once

#include "../../../Common/VexRenderCommon.h"
#include "../DX12Resource.h"
#include <DirectXTex.h>

struct DX12DescriptorMemory
{
	D3D12_GPU_DESCRIPTOR_HANDLE m_GpuDescriptorMemory;
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuDescriptorMemory;
	uint32_t m_descriptorIndex;
	uint32_t m_descriptorCount = 1;
};

class DX12DescriptorHeap
{
public:
	DX12DescriptorHeap(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	~DX12DescriptorHeap();

	void Create(D3D12_DESCRIPTOR_HEAP_TYPE descHeapType, uint32_t descHeapCapacity, bool bShaderVisible);

	DX12DescriptorMemory GetFreeDescriptorMemory();

	__forceinline ID3D12DescriptorHeap* GetDescriptorHeap() { return m_DescriptorHeap; }
	__forceinline uint32_t DescriptorHeapMaxCount() { return m_DescriptorHeapCapacity; }
	__forceinline uint32_t DescriptorHeapUsedCount() { return m_DescriptorUsed; };
private:
	ID3D12Device* m_Device;
	ID3D12GraphicsCommandList* m_CommandList;

	ID3D12DescriptorHeap* m_DescriptorHeap = nullptr;
	uint32_t m_DescriptorHeapCapacity = 0;
	uint32_t m_DescriptorUsed = 0;
	INT m_DescriptorHeapIncSize = 0;
};

class DX12ResoruceAllocator
{
public:
	DX12ResoruceAllocator();
	~DX12ResoruceAllocator();

	__forceinline void Init(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) 
	{ 
		m_device = device;

		m_shaderVisibleDescHeap = new DX12DescriptorHeap(device, cmdList);
		m_shaderVisibleDescHeap->Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, VEX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE, true);

		m_depthDescHeap = new DX12DescriptorHeap(device, cmdList);
		m_depthDescHeap->Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 256, false);

	}

	static DX12ResoruceAllocator* Get()
	{
		if (s_instance == nullptr)
		{
			s_instance = new DX12ResoruceAllocator();
		}
		return s_instance;
	}

	void BindDescHeap(ID3D12GraphicsCommandList* cmdList)
	{
		ID3D12DescriptorHeap* ppHeaps[] = { m_shaderVisibleDescHeap->GetDescriptorHeap() };
		cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	}

	void SetBindlessTexture2DHeap(UINT rootIndex, ID3D12GraphicsCommandList* cmdList)
	{
		cmdList->SetGraphicsRootDescriptorTable(rootIndex, m_shaderVisibleDescHeap->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	};

	ID3D12Resource* AllocateConstantBuffer(uint32_t CBSize);
	DX12Resource* AllocateTexture2DFromFilepath(ID3D12GraphicsCommandList* cmdList, const std::wstring& filePath, bool bMarkAsSRGB = true);
	DirectX::Blob* LoadTexture2DFromFilepath(const std::wstring& filePath, bool bMarkAsSRGB = true);
	DX12Resource* LoadTexture2DFromBinary(ID3D12GraphicsCommandList* cmdList, void* blobMemory, size_t blobByteSize, bool bMarkAsSRGB = true);
	DX12Resource* AllocateDepthTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT textureFormat, bool initSRV, bool initUAV);
	DX12Resource* AllocateTextureCubeFromFilepath(ID3D12GraphicsCommandList* cmdList, const std::wstring& filePath, bool bMarkAsSRGB = true);
	DX12Resource* AllocateHDRIFromFilepath(ID3D12GraphicsCommandList* cmdList, const std::wstring& filePath, bool bMarkAsSRGB = true);
	DX12Resource* AllocateEmptyCubemap(ID3D12GraphicsCommandList* cmdList, size_t width, size_t height, bool initSRV, bool initUAV);

private:
	inline static DX12ResoruceAllocator* s_instance = nullptr;

	ID3D12Device* m_device = nullptr;
	// one global heap for all types, contains all needed descriptors for given level
	DX12DescriptorHeap* m_shaderVisibleDescHeap = nullptr; // desc heap for all shader visible CBVs/SRVs/UAVs
	DX12DescriptorHeap* m_depthDescHeap = nullptr;
	DX12DescriptorHeap* m_RTVHeap = nullptr;

	//uint32_t m_freeIndexInsideHeap = 0;
	uint32_t m_constantBufferNumber = 0;
	uint32_t m_srvNumber = 0;
};

/*
template<typename TCacheType>
class CacheBase
{
	using TCacheTypePtr = TCacheType*;
	using TCacheBaseType = CacheBase<TCacheType>
public:

	static TCacheBaseType* Get()
	{
		if (s_instance == nullptr)
		{
			s_instance = new TCacheBaseType();
		}
		return s_instance;
	}

private:
	ID3D12Device* m_device = nullptr;
	ID3D12GraphicsCommandList* m_cmdList = nullptr;
	std::unordered_map<std::wstring, TCacheTypePtr> m_textureCache = {};
	inline static TCacheBaseType* s_instance = nullptr;
};
*/

/*
class MaterialFactory
{
public:
	static void Init(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) { s_device = device; s_cmdList0 = cmdList; };

	static Material* CreatePBRMaterialNoTextures(float roughness, float metallness, XMFLOAT3 color);
	static Material* CreatePBRMaterialWithTextures(std::wstring albedoPath, std::wstring normalPath, std::wstring roughnessPath, std::wstring metallicPath);
	static Material* CreatePBRMaterialWithTextures(Texture2D* albedoTexture, Texture2D* normalTexture, Texture2D* roughnessTexture, Texture2D* metallicTexture);

	inline static ID3D12Device* s_device = nullptr;
	inline static ID3D12GraphicsCommandList* s_cmdList0 = nullptr;
};
*/