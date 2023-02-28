#include "DX12RenderTarget.h"

DX12RenderTarget::DX12RenderTarget()
{

}

DX12RenderTarget::~DX12RenderTarget()
{

}

void DX12RenderTarget::Init(ID3D12Device* device, uint8_t colorBufferCount, bool withDepthStencilBuffer)
{
	m_device = device;


}

void DX12RenderTarget::ClearAll()
{

}

void DX12RenderTarget::ClearAllColorBuffers()
{

}

void DX12RenderTarget::ClearDepthBuffer()
{

}

void DX12RenderTarget::ClearColorBuffer(uint8_t clearIndices)
{

}

void DX12RenderTarget::SetAsRenderTarget(ID3D12CommandList* cmdList, uint8_t colorSetFlag)
{

}
