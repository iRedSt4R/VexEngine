#pragma once

#include "../../Common/VexRenderCommon.h"
#include "DX12Device.h"
#include "DX12Swapchain.h"
#include "DX12Shader.h"

class DX12RenderContext
{
	friend class DX12Renderer;

public:
	DX12RenderContext();
	~DX12RenderContext();

	void Create(ID3D12Device* m_device, std::thread::id threadID);
	void CreatePSO();
	void SetPSO(ID3D12PipelineState* pipelineState);
	void ResetCmdList();
	void SendToQueue(ID3D12CommandQueue* cmdQueue);
	//void SetPSO(ID3D12CommandList* cmdList);
	//void SetVertexBuffer(D3D12_VERTEX_BUFFER_VIEW vertexBufferView);
	//void SetIndexBuffer(D3D12_VERTEX_BUFFER_VIEW vertexBufferView);
	//void SetRenderTarget();
	//void SendToQueue(ID3D12CommandQueue* cmdQueue);
	//void Create(int16_t surfaceWidth, int16_t surfaceHeight, uint8_t backBufferCount, HWND windowHandle);
	//void BeginFrame();
	//void EndFrame();
	//void Present();

	__forceinline ID3D12GraphicsCommandList* GetCmdList() { return m_cmdList; }

private:
	ID3D12Device* m_device = nullptr;
	std::thread::id m_threadID = {};

	// renderState
	D3D12_GRAPHICS_PIPELINE_STATE_DESC m_psoDesc = {};
	D3D12_VERTEX_BUFFER_VIEW m_vertexBuffer = {};
	D3D12_INDEX_BUFFER_VIEW m_indexBuffer = {};
	D3D12_VIEWPORT m_viewport = {};
	D3D12_RECT m_scrissors = {};


	ID3D12GraphicsCommandList* m_cmdList = nullptr;
	ID3D12CommandAllocator* m_cmdAllocator[VEX_RENDER_BACK_BUFFER_COUNT] = {nullptr};
	// used to count which cmdAllocator to use
	uint8_t m_currentBackBuffer = 0;
};