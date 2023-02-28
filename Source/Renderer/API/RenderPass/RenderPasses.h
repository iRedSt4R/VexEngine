#pragma once
#include "RenderPassBase.h"
#include "../../Common/VexRenderCommon.h"
#include "../D3D12/ShaderCompiler/DX12ShaderLibrary.h"

#include <dxcapi.h>

//Passes:
class RenderPass2D : public RenderPassBase
{
public:
	void Create(ID3D12Device* device) override final;
	void BeginPass(ID3D12GraphicsCommandList* cmdList) override final;
	void RunPass(ID3D12GraphicsCommandList* cmdList) override final;
	void EndPass(ID3D12GraphicsCommandList* cmdList) override final;

private:
	// Root signature and PSO unique to this pass
	ID3D12RootSignature* m_RootSignature;
	ID3D12PipelineState* m_PipelineStateObject;

	// Shaders
	D3D12_SHADER_BYTECODE m_vsShader;
	D3D12_SHADER_BYTECODE m_psShader;
};
