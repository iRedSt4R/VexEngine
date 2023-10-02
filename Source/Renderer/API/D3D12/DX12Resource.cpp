#include "DX12Resource.h"

static uint64_t globalResourceIDCounter = 0;

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
	m_uniqueResourceID = globalResourceIDCounter++;
}

void DX12Resource::AddSRV(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_CPU, uint32_t indexInsideHeap)
{
	m_bHaveSRV = true;
	m_SRV_CPU = srvHandle_CPU;
	m_SRV = srvHandle;
	m_SRVIndexInsideShaderVisibleHeap = indexInsideHeap;
}

void DX12Resource::AddUAV(D3D12_GPU_DESCRIPTOR_HANDLE uavHandle, D3D12_CPU_DESCRIPTOR_HANDLE uavHandle_CPU, uint32_t indexInsideHeap)
{
	m_bHaveUAV = true;
	m_UAV_CPU = uavHandle_CPU;
	m_UAV = uavHandle;
	m_UAVIndexInsideShaderVisibleHeap = indexInsideHeap;
}

void DX12Resource::AddRTV(D3D12_GPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_CPU, uint32_t indexInsideHeap)
{
	m_bHaveRTV = true;
	m_RTV_CPU = rtvHandle_CPU;
	m_RTV = rtvHandle;
}

void DX12Resource::AddDSV(D3D12_GPU_DESCRIPTOR_HANDLE dsvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_CPU, uint32_t indexInsideHeap)
{
	m_bHaveDSV = true;
	m_DSV_CPU = dsvHandle_CPU;
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
