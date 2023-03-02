#pragma once

#include "DX12RenderContext.h"
#include "DX12Device.h"
#include "DX12Swapchain.h"
#include "DX12IndexedVertexBuffer.h"
#include "ShaderCompiler\DX12ShaderLibrary.h"
#include "../RenderPass/RenderPasses.h"


class DX12Renderer
{
public:
	DX12Renderer();
	~DX12Renderer();

	static DX12Renderer* Get()
	{
		if (s_renderer == nullptr)
			s_renderer = new DX12Renderer();

		return s_renderer;
	}

	void Create(int16_t surfaceWidth, int16_t surfaceHeight, uint8_t backBufferCount, HWND windowHandle);

	// Called once per frame for all contexts
	void BeginFrame();
	void EndFrame();

	//void AddRenderPass(IRenderPass* renderPass) { m_renderPasses.push_back(renderPass); }

	// Called on specific context to clear and reinit context data (usually between render passes)
	void PreDraw(uint8_t contextID);
	void EndDraw(uint8_t contextID);

	// Contexts data passing:
	__forceinline void SetIndexedVertexBuffer(uint8_t contextID, DX12IndexedVertexBuffer* indexedVertexBuffer) 
	{ 
		m_renderContexts[contextID]->m_vertexBuffer = indexedVertexBuffer->GetVertexBufferView();
		m_renderContexts[contextID]->m_indexBuffer = indexedVertexBuffer->GetIndexBufferView();
	};
	__forceinline void SetInputLayout(uint8_t contextID, D3D12_INPUT_LAYOUT_DESC inputLayout) { m_renderContexts[contextID]->m_psoDesc.InputLayout = inputLayout; }
	__forceinline void SetVertexShader(uint8_t contextID, D3D12_SHADER_BYTECODE vertexShader) { m_renderContexts[contextID]->m_psoDesc.VS = vertexShader; }
	__forceinline void SetPixelShader(uint8_t contextID, D3D12_SHADER_BYTECODE pixelShader) { m_renderContexts[contextID]->m_psoDesc.PS = pixelShader; }
	__forceinline void DrawIndexed(uint8_t contextID, uint32_t indicesCount) { m_renderContexts[contextID]->DrawIndexed(indicesCount); }

	__forceinline void SendToMainQueue(uint8_t contextID) 
	{
		m_renderContexts[contextID]->SendToQueue(m_device->GetMainCommandQueue()); 
		m_bLastExecutedContext = contextID;
	}

	// Creator funtions:
	DX12IndexedVertexBuffer* CreateIndexedVertexBuffer(int vertexBufferByteSize, int vertexBufferStride, int indexCount, void* vbData, void* ibData);


	//__forceinline void ResolveContextBarrier(uint8_t contextID, )

	__forceinline ID3D12Device* GetD3D12Device() { return m_device->GetDevice(); }
	__forceinline ID3D12GraphicsCommandList* GetContextCmdList(uint8_t contextID) { return m_renderContexts[contextID]->m_cmdList; }
	__forceinline uint8_t GetLastExecContextID() { return m_bLastExecutedContext; }

private:
	inline static DX12Renderer* s_renderer = nullptr;

	DX12ShaderLibrary* m_shaderLibrary = nullptr;

	DX12Device* m_device = nullptr;
	DX12SwapChain* m_swapChain = nullptr;
	D3D12_VIEWPORT m_viewport = {};
	D3D12_RECT m_scissors = {};
	ID3D12Fence* m_frameFence = nullptr;
	HANDLE m_frameFenceEvent = 0;
	uint64_t fenceValue = 0;

	std::vector<DX12RenderContext*> m_renderContexts = {};
	uint8_t m_bLastExecutedContext = 0;

	//std::vector<IRenderPass*> m_renderPasses;

};