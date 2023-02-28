#pragma once

#include "../../Common/VexRenderCommon.h"

class DX12Device
{
public:
	explicit DX12Device();
	~DX12Device();

	void Create();

	__forceinline ID3D12Device* GetDevice() { return m_Device; }
	__forceinline ID3D12CommandQueue* GetMainCommandQueue() { return m_MainCommandQueue; }
private:
	IDXGIAdapter1* m_adapter;
	ID3D12Device* m_Device = nullptr;
	ID3D12CommandQueue* m_MainCommandQueue = nullptr;
};