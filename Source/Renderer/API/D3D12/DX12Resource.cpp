#include "DX12Resource.h"

DX12Resource::DX12Resource()
{

}

DX12Resource::~DX12Resource()
{
	delete m_resource;
}

//void DX12Resource::AddResource(D3D12MA::Allocation* allocation)
//{
//	m_Allocation = allocation;
//}

void DX12Resource::AddResource(ID3D12Resource* resource)
{
	m_resource = resource;
}

void DX12Resource::AddSRV(DX12DescriptorMemory srvHandle)
{
	m_bHaveSRV = true;
	m_SRV = srvHandle;
}

void DX12Resource::AddUAV(DX12DescriptorMemory uavHandle)
{
	m_bHaveUAV = true;
	m_UAV = uavHandle;
}

void DX12Resource::AddRTV(DX12DescriptorMemory rtvHandle)
{
	m_bHaveRTV = true;
	m_RTV = rtvHandle;
}

void DX12Resource::AddDSV(DX12DescriptorMemory dsvHandle)
{
	m_bHaveDSV = true;
	m_DSV = dsvHandle;
}

bool DX12Resource::ChangeState(D3D12_RESOURCE_STATES newState, ID3D12GraphicsCommandList* cmdList)
{
	if (m_currentResourceState == newState)
		return false;

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resource, m_currentResourceState, newState);
	cmdList->ResourceBarrier(1, &barrier);
	m_currentResourceState = newState;

	return true;
}

//bool DX12Resource::ChangeState(D3D12_RESOURCE_STATES newState)
//{
	//if (m_CurrentResourceState == newState)
		//return false;

	//CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resource, m_CurrentResourceState, newState);
	//cmdList->ResourceBarrier(1, &barrier);

	//m_CurrentResourceState = newState;
	//return true;
//}

DX12ResourceBase::DX12ResourceBase()
{

}

DX12ResourceBase::~DX12ResourceBase()
{
	delete m_Resource;
}