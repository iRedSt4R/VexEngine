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

DX12Resource* DX12ResoruceAllocator::AllocateTexture2DFromFilepath(ID3D12GraphicsCommandList* cmdList, const std::wstring& filePath, bool bMarkAsSRGB)
{
	std::filesystem::path pp(filePath);

	DX12Resource* returnResource = new DX12Resource();
	ID3D12Resource* shaderResource;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;

	HRESULT hr;
	// Load texture using DirectXTex
	if(pp.extension() == ".dds")
		hr = DirectX::LoadFromDDSFile(filePath.c_str(), DirectX::DDS_FLAGS_NONE, &metadata, scratchImage);
	else
		hr = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, &metadata, scratchImage);

	if(bMarkAsSRGB)
		metadata.format = DirectX::MakeSRGB(metadata.format); // just add _sRGB to the dx12 format, so it will automatically apply gamma correction (?)

	if (hr != S_OK)
		return nullptr;

	// Save image as dds (and generate mipmaps)
	DirectX::ScratchImage imgWithMipMaps;
	auto bMipMapGenSuccess = DirectX::GenerateMipMaps(*scratchImage.GetImage(0, 0, 0), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_CUBIC, 8, imgWithMipMaps);


	size_t mipLevels = 0;
	if (bMipMapGenSuccess == S_OK)
	{
		metadata = imgWithMipMaps.GetMetadata();
		if (bMarkAsSRGB)
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
	if (bMipMapGenSuccess == S_OK)
		pImages = imgWithMipMaps.GetImages();
	else
		pImages = scratchImage.GetImages();

	std::vector<D3D12_SUBRESOURCE_DATA> subresources(bMipMapGenSuccess == S_OK ? imgWithMipMaps.GetImageCount() : scratchImage.GetImageCount());
	const Image* pImagess = bMipMapGenSuccess == S_OK ? imgWithMipMaps.GetImages() : scratchImage.GetImages();

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
	DX12DescriptorMemory descMemory = m_shaderVisibleDescHeap->GetFreeDescriptorMemory();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	if (bMipMapGenSuccess == S_OK)
		srvDesc.Texture2D.MipLevels = metadata.mipLevels;
	else
		srvDesc.Texture2D.MipLevels = metadata.mipLevels;
	m_device->CreateShaderResourceView(shaderResource, &srvDesc, descMemory.m_CpuDescriptorMemory);
	m_srvNumber++;

	// create final DX12Resource
	returnResource->AddResource(shaderResource);
	returnResource->AddSRV(descMemory.m_GpuDescriptorMemory, descMemory.m_CpuDescriptorMemory, descMemory.m_descriptorIndex);

	// return SRV
	return returnResource;
}

DirectX::Blob* DX12ResoruceAllocator::LoadTexture2DFromFilepath(const std::wstring& filePath, bool bMarkAsSRGB /*= true*/)
{
#define COMPRESS 1

	std::filesystem::path pp(filePath);

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;

	// Load texture using DirectXTex
	HRESULT hr = DirectX::LoadFromWICFile(filePath.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, &metadata, scratchImage);

	// Save image as dds (and generate mipmaps)
	DirectX::ScratchImage imgWithMipMaps;
	hr = DirectX::GenerateMipMaps(*scratchImage.GetImage(0, 0, 0), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_CUBIC, 8, imgWithMipMaps);

	if (hr == S_OK)
	{
#if COMPRESS
		metadata = imgWithMipMaps.GetMetadata();
		DirectX::ScratchImage imgScratch;
		DirectX::TexMetadata compressedMetadata = metadata;
		hr = DirectX::Compress(imgWithMipMaps.GetImages(), imgWithMipMaps.GetImageCount(), compressedMetadata, DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_FLAGS::TEX_COMPRESS_BC7_QUICK | DirectX::TEX_COMPRESS_PARALLEL, 0.5f, imgScratch); // | DirectX::TEX_COMPRESS_PARALLEL
		metadata = imgScratch.GetMetadata();
		if (hr != S_OK)
			__debugbreak();
#endif
		DirectX::Blob* retBlob = new DirectX::Blob();
		hr = DirectX::SaveToDDSMemory(imgScratch.GetImage(0, 0, 0), metadata.mipLevels, metadata, DDS_FLAGS_FORCE_RGB, *retBlob);
		if (hr != S_OK)
			__debugbreak();

		return retBlob;
	}
	else
	{
#if COMPRESS
		DirectX::ScratchImage imgScratch;
		hr = DirectX::Compress(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_FLAGS::TEX_COMPRESS_DEFAULT | DirectX::TEX_COMPRESS_PARALLEL, 0.5f, imgScratch);
		metadata = imgScratch.GetMetadata();

		if (hr != S_OK)
			__debugbreak();
#endif
		DirectX::Blob* retBlob = new DirectX::Blob();
		hr = DirectX::SaveToDDSMemory(imgScratch.GetImage(0, 0, 0), metadata.mipLevels, metadata, DDS_FLAGS_FORCE_RGB, *retBlob);
		if (hr != S_OK)
			__debugbreak();

		return retBlob;
	}
}

DX12Resource* DX12ResoruceAllocator::LoadTexture2DFromBinary(ID3D12GraphicsCommandList* cmdList, void* blobMemory, size_t blobByteSize, bool bMarkAsSRGB /*= true*/)
{
	DX12Resource* returnResource = new DX12Resource();
	ID3D12Resource* shaderResource;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;
	HRESULT hr = DirectX::LoadFromDDSMemory(blobMemory, blobByteSize, DDS_FLAGS_FORCE_RGB, &metadata, scratchImage);
	if(bMarkAsSRGB)
		metadata.format = DirectX::MakeSRGB(metadata.format);

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
	std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());
	const Image* pImagess = scratchImage.GetImages();

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
	DX12DescriptorMemory descMemory = m_shaderVisibleDescHeap->GetFreeDescriptorMemory();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = metadata.mipLevels;
	m_device->CreateShaderResourceView(shaderResource, &srvDesc, descMemory.m_CpuDescriptorMemory);
	m_srvNumber++;

	// create final DX12Resource
	returnResource->AddResource(shaderResource);
	returnResource->AddSRV(descMemory.m_GpuDescriptorMemory, descMemory.m_CpuDescriptorMemory, descMemory.m_descriptorIndex);

	// return SRV
	return returnResource;
}

DX12Resource* DX12ResoruceAllocator::AllocateEmptyTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT textureFormat, bool initSRV, bool initUAV, bool initRTV, int depth)
{
	DX12Resource* returnResource = new DX12Resource();
	ID3D12Resource* res;

	returnResource->SetWidthHeight(width, height);

	// resource:
	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Alignment = 0;
	resDesc.Width = (UINT)width;
	resDesc.Height = (UINT)height;
	resDesc.DepthOrArraySize = depth;
	resDesc.MipLevels = 1;
	resDesc.Format = textureFormat;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	if(initRTV) 
		resDesc.Flags = resDesc.Flags | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if(initUAV)
		resDesc.Flags = resDesc.Flags | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = textureFormat;
	clearValue.Color[0] = 0.0f;
	clearValue.Color[1] = 0.0f;
	clearValue.Color[2] = 0.0f;
	clearValue.Color[3] = 1.0f;


	auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	m_device->CreateCommittedResource(
		&heapType,
		D3D12_HEAP_FLAG_NONE, // todo: change flag to more optimized
		&resDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&clearValue,
		IID_PPV_ARGS(&res));

	returnResource->SetCurrentState(D3D12_RESOURCE_STATE_COMMON);
	returnResource->AddResource(res);

	// Create SRV
	if (initSRV)
	{
		DX12DescriptorMemory descMemory = m_shaderVisibleDescHeap->GetFreeDescriptorMemory();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray = {};
		srvDesc.Texture2DArray.ArraySize = depth;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.PlaneSlice = 0;
		srvDesc.Texture2DArray.MipLevels = 1;

		//srvDesc.Texture2D.PlaneSlice = 2;
		m_device->CreateShaderResourceView(res, &srvDesc, descMemory.m_CpuDescriptorMemory);
		m_srvNumber++;

		returnResource->AddSRV(descMemory.m_GpuDescriptorMemory, descMemory.m_CpuDescriptorMemory, descMemory.m_descriptorIndex);
	}
	if (initRTV)
	{
		DX12DescriptorMemory descMemory = m_RTVHeap->GetFreeDescriptorMemory();

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureFormat;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray = {};
		rtvDesc.Texture2DArray.ArraySize = 1;
		rtvDesc.Texture2DArray.FirstArraySlice = 0;
		rtvDesc.Texture2DArray.PlaneSlice = 0;

		m_device->CreateRenderTargetView(res, &rtvDesc, descMemory.m_CpuDescriptorMemory);

		returnResource->AddRTV(descMemory.m_GpuDescriptorMemory, descMemory.m_CpuDescriptorMemory, descMemory.m_descriptorIndex);

		for (uint32_t sliceID = 1; sliceID < depth; sliceID++)
		{
			DX12DescriptorMemory descMemory = m_RTVHeap->GetFreeDescriptorMemory();

			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = textureFormat;
			rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray = {};
			rtvDesc.Texture2DArray.ArraySize = 1;
			rtvDesc.Texture2DArray.FirstArraySlice = sliceID;
			rtvDesc.Texture2DArray.PlaneSlice = 0;

			m_device->CreateRenderTargetView(res, &rtvDesc, descMemory.m_CpuDescriptorMemory);
		}
	}

	if (initUAV)
	{
		DX12DescriptorMemory descMemory = m_shaderVisibleDescHeap->GetFreeDescriptorMemory();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = textureFormat;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

		m_device->CreateUnorderedAccessView(res, nullptr, &uavDesc, descMemory.m_CpuDescriptorMemory);

		returnResource->AddUAV(descMemory.m_GpuDescriptorMemory, descMemory.m_CpuDescriptorMemory, descMemory.m_descriptorIndex);
		m_srvNumber++;
	}

	// return SRV
	return returnResource;
}

DX12Resource* DX12ResoruceAllocator::AllocateDepthTexture2D(uint32_t width, uint32_t height, DXGI_FORMAT textureFormat, bool initSRV, bool initUAV)
{
	DX12Resource* returnResource = new DX12Resource();
	ID3D12Resource* res;

	// resource:
	D3D12_RESOURCE_DESC depthDesc = {};
	depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthDesc.Alignment = 0;
	depthDesc.Width = (UINT)width;
	depthDesc.Height = (UINT)height;
	depthDesc.DepthOrArraySize = 1;
	depthDesc.MipLevels = 1;
	depthDesc.Format = textureFormat;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	m_device->CreateCommittedResource(
		&heapType,
		D3D12_HEAP_FLAG_NONE, // todo: change flag to more optimized
		&depthDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&res));

	returnResource->SetCurrentState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
	returnResource->AddResource(res);

	// DSV:
	DX12DescriptorMemory descMemory = m_depthDescHeap->GetFreeDescriptorMemory();

	D3D12_DEPTH_STENCIL_VIEW_DESC depthView = {};
	depthView.Format = DXGI_FORMAT_D32_FLOAT;
	depthView.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthView.Flags = D3D12_DSV_FLAG_NONE;
	depthView.Texture2D.MipSlice = 0;

	m_device->CreateDepthStencilView(res, &depthView, descMemory.m_CpuDescriptorMemory);
	returnResource->AddDSV(descMemory.m_GpuDescriptorMemory, descMemory.m_CpuDescriptorMemory, descMemory.m_descriptorIndex);

	// SRV:
	if (initSRV)
	{
		DX12DescriptorMemory srvMemory = m_shaderVisibleDescHeap->GetFreeDescriptorMemory();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		m_device->CreateShaderResourceView(res, &srvDesc, srvMemory.m_CpuDescriptorMemory);
		returnResource->AddSRV(srvMemory.m_GpuDescriptorMemory, srvMemory.m_CpuDescriptorMemory, srvMemory.m_descriptorIndex);
	}

	return returnResource;
}

DX12Resource* DX12ResoruceAllocator::AllocateTextureCubeFromFilepath(ID3D12GraphicsCommandList* cmdList, const std::wstring& filePath, bool bMarkAsSRGB /*= true*/)
{
	std::filesystem::path pp(filePath);
	if (pp.extension() == ".hdr")
	{
		return AllocateHDRIFromFilepath(cmdList, filePath, bMarkAsSRGB);
	}

	DX12Resource* returnResource = new DX12Resource();
	ID3D12Resource* shaderResource;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;

	// Load cubemap dds from file
	HRESULT hr = DirectX::LoadFromDDSFile(filePath.c_str(), DDS_FLAGS_FORCE_RGB, &metadata, scratchImage);
	//HRESULT hr = DirectX::LoadFromWICFile(filePath.c_str(), WIC_FLAGS_FORCE_RGB, &metadata, scratchImage);

	//mipmaps gen
	//DirectX::ScratchImage scratchImageMips;
	//hr = DirectX::GenerateMipMaps3D(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_CUBIC, 8, scratchImageMips);
	//metadata = scratchImageMips.GetMetadata();

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
	std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());
	const Image* pImagess = scratchImage.GetImages();

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
	DX12DescriptorMemory descMemory = m_shaderVisibleDescHeap->GetFreeDescriptorMemory();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MipLevels = metadata.mipLevels;
	m_device->CreateShaderResourceView(shaderResource, &srvDesc, descMemory.m_CpuDescriptorMemory);
	m_srvNumber++;

	// create final DX12Resource
	returnResource->AddResource(shaderResource);
	returnResource->AddSRV(descMemory.m_GpuDescriptorMemory, descMemory.m_CpuDescriptorMemory, descMemory.m_descriptorIndex);

	// return SRV
	return returnResource;
}

DX12Resource* DX12ResoruceAllocator::AllocateHDRIFromFilepath(ID3D12GraphicsCommandList* cmdList, const std::wstring& filePath, bool bMarkAsSRGB /*= true*/)
{
	std::filesystem::path pp(filePath);

	DX12Resource* returnResource = new DX12Resource();
	ID3D12Resource* shaderResource;

	DirectX::TexMetadata metadata;
	DirectX::ScratchImage scratchImage;

	// Load cubemap dds from file
	HRESULT hr = DirectX::LoadFromHDRFile(filePath.c_str(), &metadata, scratchImage);
	//HRESULT hr = DirectX::LoadFromWICFile(filePath.c_str(), WIC_FLAGS_FORCE_RGB, &metadata, scratchImage);

	//mipmaps gen
	//DirectX::ScratchImage scratchImageMips;
	//hr = DirectX::GenerateMipMaps3D(scratchImage.GetImages(), scratchImage.GetImageCount(), scratchImage.GetMetadata(), DirectX::TEX_FILTER_FLAGS::TEX_FILTER_CUBIC, 8, scratchImageMips);
	//metadata = scratchImageMips.GetMetadata();

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
	std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());
	const Image* pImagess = scratchImage.GetImages();

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
	DX12DescriptorMemory descMemory = m_shaderVisibleDescHeap->GetFreeDescriptorMemory();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = metadata.mipLevels;
	m_device->CreateShaderResourceView(shaderResource, &srvDesc, descMemory.m_CpuDescriptorMemory);
	m_srvNumber++;

	// create final DX12Resource
	returnResource->AddResource(shaderResource);
	returnResource->AddSRV(descMemory.m_GpuDescriptorMemory, descMemory.m_CpuDescriptorMemory, descMemory.m_descriptorIndex);

	// return SRV
	return returnResource;
}

DX12Resource* DX12ResoruceAllocator::AllocateEmptyCubemap(ID3D12GraphicsCommandList* cmdList, size_t width, size_t height, bool initSRV, bool initUAV)
{
	DX12Resource* returnResource = new DX12Resource();
	ID3D12Resource* res;

	// resource:
	D3D12_RESOURCE_DESC depthDesc = {};
	depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthDesc.Alignment = 0;
	depthDesc.Width = (UINT)width;
	depthDesc.Height = (UINT)height;
	depthDesc.DepthOrArraySize = 1;
	depthDesc.MipLevels = 1;
	depthDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;


	auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	m_device->CreateCommittedResource(
		&heapType,
		D3D12_HEAP_FLAG_NONE, // todo: change flag to more optimized
		&depthDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&res));

	returnResource->SetCurrentState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	returnResource->AddResource(res);

	//UAV
	if (initUAV)
	{
		DX12DescriptorMemory srvMemory = m_shaderVisibleDescHeap->GetFreeDescriptorMemory();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	}

	// SRV:
	if (initSRV)
	{
		DX12DescriptorMemory srvMemory = m_shaderVisibleDescHeap->GetFreeDescriptorMemory();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		m_device->CreateShaderResourceView(res, &srvDesc, srvMemory.m_CpuDescriptorMemory);
		returnResource->AddSRV(srvMemory.m_GpuDescriptorMemory, srvMemory.m_CpuDescriptorMemory, srvMemory.m_descriptorIndex);
	}

	return returnResource;
}

//Material* MaterialFactory::CreatePBRMaterialNoTextures(float roughness, float metallness, XMFLOAT3 color)
//{
	//Material* newMaterial = new Material();
//}
