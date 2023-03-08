#pragma once

#include "../../Common/VexRenderCommon.h"
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
	void AddSRV(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_CPU);
	void AddUAV(D3D12_GPU_DESCRIPTOR_HANDLE uavHandle, D3D12_CPU_DESCRIPTOR_HANDLE uavHandle_CPU);
	void AddRTV(D3D12_GPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_CPU);
	void AddDSV(D3D12_GPU_DESCRIPTOR_HANDLE dsvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_CPU);

	// return true if barrier has to be executed, return false if resource was already in desired state (SINGLE THREADED)
	//void AssureContextState(uint8_t contextID, D3D12_RESOURCE_STATES desiredState, ID3D12GraphicsCommandList* cmdList);
	//bool ChangeContextState(uint8_t contextID, D3D12_RESOURCE_STATES newState, ID3D12GraphicsCommandList* cmdList);
	bool ChangeState(D3D12_RESOURCE_STATES newState, ID3D12GraphicsCommandList* cmdList);
	void SetCurrentState(D3D12_RESOURCE_STATES newState) { m_currentResourceState = newState; }

	__forceinline D3D12_GPU_DESCRIPTOR_HANDLE GetSRV() { return m_SRV; }
	__forceinline D3D12_GPU_DESCRIPTOR_HANDLE GetUAV() { return m_UAV; }
	__forceinline D3D12_GPU_DESCRIPTOR_HANDLE GetRTV() { return m_RTV; }
	__forceinline D3D12_GPU_DESCRIPTOR_HANDLE GetDSV() { return m_DSV; }

	__forceinline D3D12_CPU_DESCRIPTOR_HANDLE GetSRV_CPU() { return m_SRV_CPU; }
	__forceinline D3D12_CPU_DESCRIPTOR_HANDLE GetUAV_CPU() { return m_UAV_CPU; }
	__forceinline D3D12_CPU_DESCRIPTOR_HANDLE GetRTV_CPU() { return m_RTV_CPU; }
	__forceinline D3D12_CPU_DESCRIPTOR_HANDLE GetDSV_CPU() { return m_DSV_CPU; }

	//__forceinline uint32_t GetSRVIndex() { return m_SRV; }
	//__forceinline uint32_t GetUAVIndex() { return m_UAV; }
	//__forceinline uint32_t GetRTVIndex() { return m_RTV; }
	//__forceinline uint32_t GetDSVIndex() { return m_DSV; }

	//__forceinline ID3D12Resource* GetResource() { return m_Allocation->GetResource(); }
	//__forceinline D3D12MA::Allocation* GetAllocation() { return m_Allocation; }
	__forceinline ID3D12Resource* GetResource() { return m_resource; }
private:

	bool m_bHaveSRV = false;
	bool m_bHaveUAV = false;
	bool m_bHaveRTV = false;
	bool m_bHaveDSV = false;

	D3D12_GPU_DESCRIPTOR_HANDLE m_SRV = {};
	D3D12_GPU_DESCRIPTOR_HANDLE m_UAV = {};
	D3D12_GPU_DESCRIPTOR_HANDLE m_RTV = {};
	D3D12_GPU_DESCRIPTOR_HANDLE m_DSV = {};

	D3D12_CPU_DESCRIPTOR_HANDLE m_SRV_CPU = {};
	D3D12_CPU_DESCRIPTOR_HANDLE m_UAV_CPU = {};
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTV_CPU = {};
	D3D12_CPU_DESCRIPTOR_HANDLE m_DSV_CPU = {};

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