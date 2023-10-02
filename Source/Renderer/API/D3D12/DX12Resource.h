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
	void AddSRV(D3D12_GPU_DESCRIPTOR_HANDLE srvHandle, D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_CPU, uint32_t indexInsideHeap);
	void AddUAV(D3D12_GPU_DESCRIPTOR_HANDLE uavHandle, D3D12_CPU_DESCRIPTOR_HANDLE uavHandle_CPU, uint32_t indexInsideHeap);
	void AddRTV(D3D12_GPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_CPU, uint32_t indexInsideHeap);
	void AddDSV(D3D12_GPU_DESCRIPTOR_HANDLE dsvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_CPU, uint32_t indexInsideHeap);

	void SetWidthHeight(uint32_t width, uint32_t height) { m_width = width; m_height = height; }

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

	__forceinline uint32_t GetSRVIndexInsideHeap() { return m_SRVIndexInsideShaderVisibleHeap; }
	__forceinline uint32_t GetUAVIndexInsideHeap() { return m_UAVIndexInsideShaderVisibleHeap; }
	__forceinline uint32_t GetRTVIndexInsideHeap() { return m_RTVIndexInsideHeap; }
	__forceinline uint32_t GetDSVIndexInsideHeap() { return m_DSVIndexInsideHeap; }

	__forceinline ID3D12Resource* GetResource() { return m_resource; }

	__forceinline uint32_t GetWidth() { return m_width; }
	__forceinline uint32_t GetHeight() { return m_height; }

	__forceinline uint32_t SetResourceID(uint64_t resourceID) { m_uniqueResourceID = resourceID; }
	__forceinline uint32_t GetResourceID() { return m_uniqueResourceID; }
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

	uint32_t m_SRVIndexInsideShaderVisibleHeap = 0;
	uint32_t m_UAVIndexInsideShaderVisibleHeap = 0;
	uint32_t m_CBVIndexInsideShaderVisibleHeap = 0;
	uint32_t m_DSVIndexInsideHeap = 0;
	uint32_t m_RTVIndexInsideHeap = 0;

	ID3D12Resource* m_resource = nullptr;
	D3D12_RESOURCE_STATES m_currentResourceState = D3D12_RESOURCE_STATE_COMMON;

	uint32_t m_width = 0;
	uint32_t m_height = 0;

	// unique resource id for the lifetime of the game
	uint64_t m_uniqueResourceID;
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