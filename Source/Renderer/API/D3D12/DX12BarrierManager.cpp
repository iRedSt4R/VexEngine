#include "DX12BarrierManager.h"

DX12BarrierManager::DX12BarrierManager()
{

}

DX12BarrierManager::~DX12BarrierManager()
{

}

void DX12BarrierManager::Init(ID3D12Device* device)
{
	m_device = device;

	// cmdList for unknown barriers resolve
	m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_barrierCmdAllocator));
	m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_barrierCmdAllocator, nullptr, IID_PPV_ARGS(&m_barrierCmdList));
	m_barrierCmdList->Close();
}
