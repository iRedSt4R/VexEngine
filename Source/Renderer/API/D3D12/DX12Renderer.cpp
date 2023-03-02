#include "DX12Renderer.h"

DX12Renderer::DX12Renderer()
{
	m_shaderLibrary = DX12ShaderLibrary::Get();
}

DX12Renderer::~DX12Renderer()
{
	
}

void DX12Renderer::Create(int16_t surfaceWidth, int16_t surfaceHeight, uint8_t backBufferCount, HWND windowHandle)
{
	// device
	m_device = new DX12Device();
	m_device->Create();

	// swampchain
	m_swapChain = new DX12SwapChain();
	m_swapChain->Create(m_device->GetDevice(), m_device->GetMainCommandQueue(), surfaceWidth, surfaceHeight, 2, windowHandle);

	m_renderContexts.push_back(new DX12RenderContext());
	m_renderContexts[0]->Create(m_device->GetDevice(), std::this_thread::get_id());
	m_renderContexts[0]->m_currentBackBuffer = m_swapChain->GetCurrentBackBufferIndex();

	m_device->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_frameFence));
	m_frameFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	// fullscreen viewport and scissors
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = m_swapChain->GetBackBufferWidth();
	m_viewport.Height = m_swapChain->GetBackBufferHeight();
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	m_scissors.left = 0;
	m_scissors.top = 0;
	m_scissors.right = m_swapChain->GetBackBufferWidth();
	m_scissors.bottom = m_swapChain->GetBackBufferHeight();
}

void DX12Renderer::BeginFrame()
{
	auto backBuffer = m_swapChain->GetCurrentBackBufferRenderTarget();

	for (auto& ctx : m_renderContexts)
	{
		ctx->ResetCmdList();
	}

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_renderContexts[0]->m_cmdList->ResourceBarrier(1, &barrier);
	m_swapChain->ClearCurrentBackBuffer(m_renderContexts[0]->m_cmdList);

	// set render target
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_swapChain->GetBackBufferDescriptorHandle(), m_swapChain->GetCurrentBackBufferIndex(), m_device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	auto depthView = m_swapChain->GetDepthStencilView();
	m_renderContexts[0]->m_cmdList->OMSetRenderTargets(1, &rtv, FALSE, &depthView);

	// set fullscreen viewport and scissors
	m_renderContexts[0]->m_cmdList->RSSetViewports(1, &m_viewport);
	m_renderContexts[0]->m_cmdList->RSSetScissorRects(1, &m_scissors);
}

void DX12Renderer::EndFrame()
{
	auto backBuffer = m_swapChain->GetCurrentBackBufferRenderTarget();

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_renderContexts[0]->m_cmdList->ResourceBarrier(1, &barrier);

	for (auto& ctx : m_renderContexts)
	{
		ctx->m_cmdList->Close();
	}

	// Wait for previous frame on gpu
	if (m_frameFence->GetCompletedValue() < fenceValue)
	{
		m_frameFence->SetEventOnCompletion(fenceValue, m_frameFenceEvent);
		::WaitForSingleObject(m_frameFenceEvent, INFINITE);
	}
	fenceValue++;

	m_renderContexts[0]->SendToQueue(m_device->GetMainCommandQueue());
	m_device->GetMainCommandQueue()->Signal(m_frameFence, fenceValue);

	m_swapChain->Present();

	m_renderContexts[0]->m_currentBackBuffer = m_swapChain->GetCurrentBackBufferIndex();
}

void DX12Renderer::PreDraw(uint8_t contextID)
{

}

void DX12Renderer::EndDraw(uint8_t contextID)
{

}

DX12IndexedVertexBuffer* DX12Renderer::CreateIndexedVertexBuffer(int vertexBufferByteSize, int vertexBufferStride, int indexCount, void* vbData, void* ibData)
{
	DX12IndexedVertexBuffer* retBuffer = new DX12IndexedVertexBuffer(m_device->GetDevice());
	retBuffer->Create(m_renderContexts[0]->m_cmdList, vertexBufferByteSize, vertexBufferStride, indexCount, vbData, ibData);
	return retBuffer;
}
