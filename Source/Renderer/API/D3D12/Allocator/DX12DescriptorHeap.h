#pragma once

#include "../../../Common/VexRenderCommon.h"
#include "../DX12Resource.h"

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
		m_shaderVisibleDescHeap->Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024, true);

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
	ID3D12Resource* AllocateConstantBuffer(uint32_t CBSize);
	DX12Resource* AllocateTexture2DFromFilepath(ID3D12GraphicsCommandList* cmdList, const std::wstring& filePath);
	DX12Resource* AllocateDepthTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT textureFormat, bool initSRV, bool initUAV);

private:
	inline static DX12ResoruceAllocator* s_instance = nullptr;

	ID3D12Device* m_device = nullptr;
	// one global heap for all types, contains all needed descriptors for given level
	DX12DescriptorHeap* m_shaderVisibleDescHeap = nullptr; // desc heap for all shader visible CBVs/SRVs/UAVs
	DX12DescriptorHeap* m_depthDescHeap = nullptr;
	DX12DescriptorHeap* m_RTVHeap = nullptr;

	uint32_t m_constantBufferNumber = 0;
	uint32_t m_srvNumber = 0;
};