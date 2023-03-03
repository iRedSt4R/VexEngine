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
	std::filesystem::path pp(filePath);

	DX12Resource* returnResource = new DX12Resource();
	ID3D12Resource* shaderResource;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;

	// Load texture using DirectXTex
	HRESULT hr = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, &metadata, scratchImage);
	metadata.format = DirectX::MakeSRGB(metadata.format); // just add _sRGB to the dx12 format, so it will automatically apply gamma correction (?)

	// Save image as dds (and generate mipmaps)
	DirectX::ScratchImage imgWithMipMaps;
	auto hrr = DirectX::GenerateMipMaps(*scratchImage.GetImage(0, 0, 0), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_CUBIC, 8, imgWithMipMaps);


	size_t mipLevels = 0;
	if (hrr == S_OK)
	{
		metadata = imgWithMipMaps.GetMetadata();
		metadata.format = DirectX::MakeSRGB(metadata.format);
		//metadata.mipLevels = 4;
		mipLevels = 8;
	}
	//DirectX::ScratchImage imgInBC2;
	//hr = DirectX::Convert(imgWithMipMaps.GetImages(), imgWithMipMaps.GetImageCount(), srcMetadata, DXGI_FORMAT_BC2_UNORM, DirectX::TEX_FILTER_FLAGS::TEX_FILTER_CUBIC, 0.5f, imgInBC2);
	//hr = DirectX::Compress(imgWithMipMaps.GetImages(), imgWithMipMaps.GetImageCount(), imgWithMipMaps.GetMetadata(), true ? DXGI_FORMAT_BC3_UNORM : DXGI_FORMAT_BC1_UNORM, DirectX::TEX_COMPRESS_FLAGS::TEX_COMPRESS_DEFAULT | DirectX::TEX_COMPRESS_PARALLEL, 0.5f, imgInBC2);

	//auto fileToWrite = pp.filename().wstring().c_str();
	//pp.replace_extension(".dds");
	//hr = DirectX::SaveToDDSFile(imgWithMipMaps.GetImages(), imgWithMipMaps.GetImageCount(), imgWithMipMaps.GetMetadata(), DDS_FLAGS_NONE, pp.filename().wstring().c_str());


	// Fill the resource desc
	D3D12_RESOURCE_DESC textureDesc{};
	textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		static_cast<UINT64>(metadata.width),
		static_cast<UINT>(metadata.height),
		static_cast<UINT16>(metadata.arraySize),
		static_cast<UINT16>(metadata.mipLevels)
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
	const DirectX::Image* pImages;
	if (hrr == S_OK)
		pImages = imgWithMipMaps.GetImages();
	else
		pImages = scratchImage.GetImages();

	std::vector<D3D12_SUBRESOURCE_DATA> subresources(hrr == S_OK ? imgWithMipMaps.GetImageCount() : scratchImage.GetImageCount());
	const Image* pImagess = hrr == S_OK ? imgWithMipMaps.GetImages() : scratchImage.GetImages();

	for (int i = 0; i < subresources.size(); ++i) {

		auto& subresource = subresources[i];
		subresource.RowPitch = pImagess[i].rowPitch;
		subresource.SlicePitch = pImagess[i].slicePitch;
		subresource.pData = pImagess[i].pixels;
	}

	UINT64 requiredSize = GetRequiredIntermediateSize(shaderResource, 0, subresources.size());

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
	UpdateSubresources(cmdList, shaderResource, intermediateResource, 0, 0, subresources.size(), subresources.data());

	// Change resource state from copy_dest to pixel_shader_resource
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(shaderResource, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrier);

	// Create SRV from resource
	DX12DescriptorMemory descMemory = m_descHeap->GetFreeDescriptorMemory();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	if (hrr == S_OK)
		srvDesc.Texture2D.MipLevels = metadata.mipLevels;
	else
		srvDesc.Texture2D.MipLevels = metadata.mipLevels;
	m_device->CreateShaderResourceView(shaderResource, &srvDesc, descMemory.m_CpuDescriptorMemory);
	m_srvNumber++;

	// create final DX12Resource
	returnResource->AddResource(shaderResource);
	returnResource->AddSRV(descMemory.m_GpuDescriptorMemory);

	// return SRV
	return returnResource;
}