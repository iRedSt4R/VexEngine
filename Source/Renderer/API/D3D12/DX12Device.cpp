#include "DX12Device.h"
#include <dxgi1_6.h>

DX12Device::DX12Device()
{

}

DX12Device::~DX12Device()
{

}

void DX12Device::Create()
{
	HRESULT hr;

#if VEX_RENDER_USE_VALIDATOR
	ID3D12Debug* debugInterface;
	D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
	debugInterface->EnableDebugLayer();

#if 0
	CComPtr<ID3D12Debug> spDebugController0;
	CComPtr<ID3D12Debug1> spDebugController1;
	D3D12GetDebugInterface(IID_PPV_ARGS(&spDebugController0));
	spDebugController0->QueryInterface(IID_PPV_ARGS(&spDebugController1));
	spDebugController1->SetEnableGPUBasedValidation(true);
#endif
#endif

	// Factory
	ComPtr<IDXGIFactory4> dxgiFactory;
	UINT createFactoryFlags = 0;
	CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory));

	// Enumerating adapters
	IDXGIAdapter1* dxgiAdapter1;
	SIZE_T maxDedicatedVideoMemory = 0;
	for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
		dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

		// Pick adapter with the most GPU memory
		if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
			SUCCEEDED(D3D12CreateDevice(dxgiAdapter1,
				D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr)) &&
			dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory)
		{
			maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
			m_adapter = dxgiAdapter1;
		}
	}

	// Create Device from best adapter
	hr = D3D12CreateDevice(m_adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_Device));
	CHECK_HRESULT(hr);

	// Create Command Queue
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	hr = m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_MainCommandQueue));
	CHECK_HRESULT(hr);
}
