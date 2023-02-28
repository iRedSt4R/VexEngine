#pragma once
#include "../../Common/VexRenderCommon.h"

class DX12SwapChain
{
public:
	DX12SwapChain();
	~DX12SwapChain();

	void Create(ID3D12Device* device, ID3D12CommandQueue* cmdQueue, uint32_t width, uint32_t height, int bufferCount, HWND windowHandle, DXGI_FORMAT backBuffertFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	void ResizeSwapChain(int width, int height);
	int Present();
	void ClearCurrentBackBuffer(ID3D12GraphicsCommandList* cmdList);


	__forceinline uint8_t GetBufferCount() { return m_BufferCount; }
	__forceinline IDXGISwapChain* GetD3D12SwapChain() { return m_SwapChain; }
	__forceinline ID3D12Resource* GetCurrentBackBufferRenderTarget() { return m_RTResources[m_CurrentBackBufferIndex]; }
	__forceinline uint8_t GetCurrentBackBufferIndex() { return m_CurrentBackBufferIndex; }
	__forceinline D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferDescriptorHandle() { return m_RTVHeapStartHandle; }
	__forceinline D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferHandle()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtHandle(m_RTVHeapStartHandle, m_CurrentBackBufferIndex, m_RTVDescriptorHeapIncrementSize);
		return rtHandle;
	}
	__forceinline D3D12_RESOURCE_BARRIER ChangeCurrentBackBufferState(D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_RTResources[m_CurrentBackBufferIndex], oldState, newState);
		return barrier;
	}

	__forceinline D3D12_RESOURCE_BARRIER ChangeBackBufferState(uint8_t backBufferNumber, D3D12_RESOURCE_STATES oldState, D3D12_RESOURCE_STATES newState)
	{
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_RTResources[backBufferNumber], oldState, newState);
		return barrier;
	}

	__forceinline uint16_t GetBackBufferWidth() { return m_BackBufferWidth; }
	__forceinline uint16_t GetBackBufferHeight() { return m_BackBufferHeight; }

private:
	ID3D12Device* m_Device = nullptr;
	IDXGISwapChain4* m_SwapChain = nullptr;
	ID3D12Resource** m_RTResources = { nullptr };
	ID3D12DescriptorHeap* m_RTVDescriptorHeap = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHeapStartHandle = {};

	uint8_t m_BufferCount = 2;
	uint8_t m_CurrentBackBufferIndex = 0;
	uint16_t m_BackBufferWidth = 800;
	uint16_t m_BackBufferHeight = 600;
	UINT m_RTVDescriptorHeapIncrementSize = 0;
	HWND m_WindowHandle = 0;
};
