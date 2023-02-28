#pragma once

#include "../../../Common/VexRenderCommon.h"

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

};