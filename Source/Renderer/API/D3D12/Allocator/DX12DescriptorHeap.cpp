#include "DX12DescriptorHeap.h"

DX12DescriptorHeap::DX12DescriptorHeap(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
	:m_Device(device), m_CommandList(commandList)
{

}

DX12DescriptorHeap::~DX12DescriptorHeap()
{

}

void DX12DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE descHeapType, uint32_t descHeapCapacity, bool bShaderVisible)
{
	HRESULT hr;

	m_DescriptorHeapIncSize = m_Device->GetDescriptorHandleIncrementSize(descHeapType);

	D3D12_DESCRIPTOR_HEAP_DESC depthDescHeapDesc = {};
	depthDescHeapDesc.NumDescriptors = descHeapCapacity;
	depthDescHeapDesc.Type = descHeapType;
	depthDescHeapDesc.Flags = bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = m_Device->CreateDescriptorHeap(&depthDescHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
}

DX12DescriptorMemory DX12DescriptorHeap::GetFreeDescriptorMemory()
{
	DX12DescriptorMemory outDescMemory;
	outDescMemory.m_CpuDescriptorMemory.ptr = (m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + (m_DescriptorHeapIncSize * m_DescriptorUsed));
	outDescMemory.m_GpuDescriptorMemory.ptr = (m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr + (m_DescriptorHeapIncSize * m_DescriptorUsed));
	outDescMemory.m_descriptorIndex = m_DescriptorUsed;

	m_DescriptorUsed++;

	return outDescMemory;
}

DX12ResoruceAllocator::DX12ResoruceAllocator()
{

}

DX12ResoruceAllocator::~DX12ResoruceAllocator()
{

}

ID3D12Resource* DX12ResoruceAllocator::AllocateConstantBuffer(uint32_t CBSize)
{
	uint32_t m_ByteSizeAlignedResource = 64 * 1024;
	ID3D12Resource* returnConstantBuffer;


	auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_ByteSizeAlignedResource);

	m_device->CreateCommittedResource(
		&heapType,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&returnConstantBuffer));

	m_constantBufferNumber++;

	return returnConstantBuffer;
}