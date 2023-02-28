#include "DX12RenderContext.h"

DX12RenderContext::DX12RenderContext()
{

}

DX12RenderContext::~DX12RenderContext()
{

}

void DX12RenderContext::Create(ID3D12Device* device, std::thread::id threadID)
{
	m_device = device;
	m_threadID = threadID;

	for (int i = 0; i < VEX_RENDER_BACK_BUFFER_COUNT; ++i)
	{
		m_device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator[i]));
	}
	m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator[0], nullptr, IID_PPV_ARGS(&m_cmdList));
	m_cmdList->Close();

	ResetCmdList();
}

void DX12RenderContext::CreatePSO()
{

}

void DX12RenderContext::SendToQueue(ID3D12CommandQueue* cmdQueue)
{
	ID3D12CommandList* const commandLists[] = {
		m_cmdList
	};

	cmdQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void DX12RenderContext::DrawIndexed(uint32_t indicesCount)
{
	m_cmdList->DrawIndexedInstanced(indicesCount, 1, 0, 0, 0);
}

void DX12RenderContext::ResetCmdList()
{
	auto commandAllocator = m_cmdAllocator[m_currentBackBuffer];
	commandAllocator->Reset();
	m_cmdList->Reset(commandAllocator, nullptr);
}