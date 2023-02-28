#pragma once

#include "../../Common/VexRenderCommon.h"

class DX12BarrierManager
{
public:
	DX12BarrierManager();
	~DX12BarrierManager();

	static DX12BarrierManager* Get()
	{
		if (s_instance == nullptr)
		{
			s_instance = new DX12BarrierManager();
		}

		return s_instance;
	}

	void Init(ID3D12Device* device);
	void ChangeContextState();
	//void AssureContextState()

private:
	inline static DX12BarrierManager* s_instance = nullptr;

	ID3D12Device* m_device = nullptr;

	// command list used to resolve unknown barriers between cmdLists execution
	ID3D12GraphicsCommandList* m_barrierCmdList = nullptr;
	ID3D12CommandAllocator* m_barrierCmdAllocator = nullptr;

	//D3D12_RESOURCE_STATES m_contextsState[8] = { D3D12_RESOURCE_STATE_COMMON };
	//bool m_contextsState[8] = { false };
};