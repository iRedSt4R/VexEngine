#include "RenderPasses.h"

#include "../D3D12/ShaderCompiler/DX12ShaderLibrary.h"

void RenderPass2D::Create(ID3D12Device* device)
{
	m_device = device;
	// vertex layout
	D3D12_INPUT_ELEMENT_DESC inputLayout_PosCol[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = sizeof(inputLayout_PosCol) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout_PosCol;

	// vertex shader:
	m_vsShader = DX12ShaderLibrary::Get()->GetVertexShader(L"../../Source/Renderer/Shaders/2D/2d_vs_color.hlsl");
	m_psShader = DX12ShaderLibrary::Get()->GetPixelShader(L"../../Source/Renderer/Shaders/2D/2d_ps_color.hlsl");

	// root signature
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ID3DBlob* signature;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr);

	m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));

	// createPSO:
	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.pRootSignature = m_RootSignature;
	psoDesc.VS = m_vsShader;
	psoDesc.PS = m_psShader;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc = sampleDesc;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.NumRenderTargets = 1;

	// create the PSO
	HRESULT hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineStateObject));
}

void RenderPass2D::BeginPass(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(m_PipelineStateObject);
	cmdList->SetGraphicsRootSignature(m_RootSignature);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void RenderPass2D::RunPass(ID3D12GraphicsCommandList* cmdList)
{
	for (auto& vertexBuffer : m_vertexBuffers)
	{
		cmdList->IASetVertexBuffers(0, 1, &vertexBuffer->GetVertexBufferView());
		cmdList->IASetIndexBuffer(&vertexBuffer->GetIndexBufferView());
		cmdList->DrawIndexedInstanced(vertexBuffer->GetVertexCount(), 1, 0, 0, 0);
	}
}

void RenderPass2D::EndPass(ID3D12GraphicsCommandList* cmdList)
{

}