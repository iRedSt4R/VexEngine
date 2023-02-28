#pragma once

#include "../../Common/VexRenderCommon.h"
#include "Allocator/DX12DescriptorHeap.h"
#include "Allocator/D3D12MemAlloc.h"

enum class EResourceViewType
{
	SRV,
	UAV,
	RTV,
	DSV
};

// m_XXXIndex = global index inside descriptor heap, pass this index to shader, so shader know where to access this resource view (in bindless rendering model)
class DX12Resource
{
public:
	DX12Resource();
	~DX12Resource();

	//void AddResource(D3D12MA::Allocation* allocation);
	void AddResource(ID3D12Resource* resource);
	void AddSRV(DX12DescriptorMemory srvHandle);
	void AddUAV(DX12DescriptorMemory uavHandle);
	void AddRTV(DX12DescriptorMemory rtvHandle);
	void AddDSV(DX12DescriptorMemory dsvHandle);

	// return true if barrier has to be executed, return false if resource was already in desired state (SINGLE THREADED)
	//void AssureContextState(uint8_t contextID, D3D12_RESOURCE_STATES desiredState, ID3D12GraphicsCommandList* cmdList);
	//bool ChangeContextState(uint8_t contextID, D3D12_RESOURCE_STATES newState, ID3D12GraphicsCommandList* cmdList);
	bool ChangeState(D3D12_RESOURCE_STATES newState, ID3D12GraphicsCommandList* cmdList);
	void SetCurrentState(D3D12_RESOURCE_STATES newState) { m_currentResourceState = newState; }

	__forceinline D3D12_GPU_DESCRIPTOR_HANDLE GetSRV() { return m_SRV.m_GpuDescriptorMemory; }
	__forceinline D3D12_GPU_DESCRIPTOR_HANDLE GetUAV() { return m_UAV.m_GpuDescriptorMemory; }
	__forceinline D3D12_GPU_DESCRIPTOR_HANDLE GetRTV() { return m_RTV.m_GpuDescriptorMemory; }
	__forceinline D3D12_GPU_DESCRIPTOR_HANDLE GetDSV() { return m_DSV.m_GpuDescriptorMemory; }

	__forceinline uint32_t GetSRVIndex() { return m_SRV.m_descriptorIndex; }
	__forceinline uint32_t GetUAVIndex() { return m_UAV.m_descriptorIndex; }
	__forceinline uint32_t GetRTVIndex() { return m_RTV.m_descriptorIndex; }
	__forceinline uint32_t GetDSVIndex() { return m_DSV.m_descriptorIndex; }

	//__forceinline ID3D12Resource* GetResource() { return m_Allocation->GetResource(); }
	//__forceinline D3D12MA::Allocation* GetAllocation() { return m_Allocation; }
	__forceinline ID3D12Resource* GetResource() { return m_resource; }
private:

	bool m_bHaveSRV = false;
	bool m_bHaveUAV = false;
	bool m_bHaveRTV = false;
	bool m_bHaveDSV = false;

	DX12DescriptorMemory m_SRV = {};
	DX12DescriptorMemory m_UAV = {};
	DX12DescriptorMemory m_RTV = {};
	DX12DescriptorMemory m_DSV = {};

	ID3D12Resource* m_resource = nullptr;
	D3D12_RESOURCE_STATES m_currentResourceState = D3D12_RESOURCE_STATE_COMMON;
};

class DX12ResourceBase
{
public:
	DX12ResourceBase();
	virtual ~DX12ResourceBase();

	__forceinline DX12Resource* GetDX12Resource() { return m_Resource; }
protected:
	DX12Resource* m_Resource = nullptr;
};