#include "DX12SwapChain.h"

DX12SwapChain::DX12SwapChain()
{

}

DX12SwapChain::~DX12SwapChain()
{

}

void DX12SwapChain::Create(ID3D12Device* device, ID3D12CommandQueue* cmdQueue, uint32_t width, uint32_t height, int bufferCount, HWND windowHandle, DXGI_FORMAT backBuffertFormat)
{
	m_Device = device;

	HRESULT res;

	m_BackBufferWidth = width;
	m_BackBufferHeight = height;
	m_BufferCount = bufferCount;
	m_WindowHandle = windowHandle;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = m_BackBufferWidth;
	swapChainDesc.Height = m_BackBufferHeight;
	swapChainDesc.Format = backBuffertFormat;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 }; // No sampling for now
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = m_BufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING; // TODO: Not all devices will support this, better detect if possible

	IDXGIFactory4* dxgiFactory;
	CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory));
	IDXGISwapChain1* swapChain1;
	res = dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue,
		windowHandle,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1);

	//assert(res == S_OK, "Failed creating swap chain");
	swapChain1->QueryInterface(IID_PPV_ARGS(&m_SwapChain));

	dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);

	// Descriptor Heap for RTVs
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = m_BufferCount;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_RTVDescriptorHeap));

	//Descriptor Heap for RTV descriptors
	auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_RTVDescriptorHeapIncrementSize = rtvDescriptorSize;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	m_RTVHeapStartHandle = rtvHandle;

	m_RTResources = new ID3D12Resource* [bufferCount];

	// Create color RTV for every backbuffer
	for (int i = 0; i < m_BufferCount; ++i)
	{
		ID3D12Resource* backBuffer;
		m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		device->CreateRenderTargetView(backBuffer, &rtvDesc, rtvHandle);
		m_RTResources[i] = backBuffer;

		rtvHandle.Offset(rtvDescriptorSize);
	}

	// Create dsv for swapchain
	D3D12_DESCRIPTOR_HEAP_DESC depthDescHeapDesc = {};
	depthDescHeapDesc.NumDescriptors = 1;
	depthDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	depthDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	m_Device->CreateDescriptorHeap(&depthDescHeapDesc, IID_PPV_ARGS(&m_dsvHeap));

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
	depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;


	auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto textDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	m_Device->CreateCommittedResource(
		&heapType,
		D3D12_HEAP_FLAG_NONE,
		&textDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_depthStencilResource)
	);
	m_dsvHeap->SetName(L"Depth/Stencil Resource Heap");

	m_Device->CreateDepthStencilView(m_depthStencilResource, &depthStencilDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

int DX12SwapChain::Present()
{
	//Present, no sync, allow tearing (must support FreeSync/Gsync for now)
	UINT syncInterval = 0;
	UINT presentFlags = DXGI_PRESENT_ALLOW_TEARING;
	m_SwapChain->Present(syncInterval, presentFlags);

	//After present get next back buffer index;
	m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

	return m_CurrentBackBufferIndex;
}

void DX12SwapChain::ClearCurrentBackBuffer(ID3D12GraphicsCommandList* cmdList)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	FLOAT clearColor[] = { 0.0f, 0.1f, 0.5f, 1.0f };
	cmdList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void DX12SwapChain::ResizeSwapChain(int width, int height)
{
	for (int swapChainImage = 0; swapChainImage < m_BufferCount; swapChainImage++)
	{
		SAFE_RELEASE(m_RTResources[swapChainImage]);
	}

	m_SwapChain->ResizeBuffers(m_BufferCount, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

	auto rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_RTVDescriptorHeapIncrementSize = rtvDescriptorSize;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	m_RTVHeapStartHandle = rtvHandle;

	for (int i = 0; i < m_BufferCount; ++i)
	{
		ID3D12Resource* backBuffer;
		m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));

		m_Device->CreateRenderTargetView(backBuffer, nullptr, rtvHandle);
		m_RTResources[i] = backBuffer;

		rtvHandle.Offset(rtvDescriptorSize);
	}

	m_CurrentBackBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();
}
