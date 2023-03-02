#include "DX12DescriptorHeap.h"
#include <DirectXTex.h>

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

DX12Resource* DX12ResoruceAllocator::AllocateTexture2DFromFilepath(ID3D12GraphicsCommandList* cmdList, const std::wstring& filePath)
{
	DX12Resource* returnResource = new DX12Resource();
	ID3D12Resource* shaderResource;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;

	// Load texture using DirectXTex
	HRESULT hr = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, &metadata, scratchImage);
	metadata.format = DirectX::MakeSRGB(metadata.format); // just add _sRGB to the dx12 format, so it will automatically apply gamma correction (?)

	// Fill the resource desc
	D3D12_RESOURCE_DESC textureDesc{};
	textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		static_cast<UINT64>(metadata.width),
		static_cast<UINT>(metadata.height),
		static_cast<UINT16>(metadata.arraySize)
	);

	// Creating committed resource from desc
	auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	hr = m_device->CreateCommittedResource(
		&heapType,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&shaderResource));

	// subresource info for copying
	const DirectX::Image* pImages = scratchImage.GetImages();
	D3D12_SUBRESOURCE_DATA subresource;
	subresource.RowPitch = pImages[0].rowPitch;
	subresource.SlicePitch = pImages[0].slicePitch;
	subresource.pData = pImages[0].pixels;

	UINT64 requiredSize = GetRequiredIntermediateSize(shaderResource, 0, 1);

	ID3D12Resource* intermediateResource;
	heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto bufferType = CD3DX12_RESOURCE_DESC::Buffer(requiredSize);
	hr = m_device->CreateCommittedResource(
		&heapType,
		D3D12_HEAP_FLAG_NONE,
		&bufferType,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&intermediateResource)
	);

	if (hr != S_OK)
	{
		__debugbreak();
	}

	// Update data through upload heap
	UpdateSubresources(cmdList, shaderResource, intermediateResource, 0, 0, 1, &subresource);

	// Change resource state from copy_dest to pixel_shader_resource
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(shaderResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);

	// Create SRV from resource
	DX12DescriptorMemory descMemory = m_descHeap->GetFreeDescriptorMemory();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	m_device->CreateShaderResourceView(shaderResource, &srvDesc, descMemory.m_CpuDescriptorMemory);
	m_srvNumber++;

	// create final DX12Resource
	returnResource->AddResource(shaderResource);
	returnResource->AddSRV(descMemory.m_GpuDescriptorMemory);

	// return SRV
	return returnResource;
}