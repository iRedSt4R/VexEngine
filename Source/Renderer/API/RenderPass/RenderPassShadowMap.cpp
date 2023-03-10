#include "RenderPasses.h"

static bool bInit = false;

void RenderPassShadowMap::Create(DX12Renderer* renderer)
{
	m_renderer = renderer;
	m_device = renderer->GetD3D12Device();
	m_shadowCamera = new ShadowCamera();
	m_shadowCameraCB = new ConstantBuffer<ShadowCameraCB>(m_device);

	D3D12_INPUT_ELEMENT_DESC inputLayout_PosCol[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = sizeof(inputLayout_PosCol) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout_PosCol;

	// vertex shader:
	m_vsShader = DX12ShaderLibrary::Get()->GetVertexShader(L"../../Source/Renderer/Shaders/2D/vs_depth_pass.hlsl");

	// root signature
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;

	D3D12_ROOT_PARAMETER  rootParameters[1];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = rootCBVDescriptor;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = _countof(rootParameters);
	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

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
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 0;
	psoDesc.SampleDesc = sampleDesc;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// create the PSO
	HRESULT hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineStateObject));
}

void RenderPassShadowMap::BeginPass(uint8_t contextID)
{
	ID3D12GraphicsCommandList* cmdList = m_renderer->GetContextCmdList(contextID);

	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 3840.f;
	viewport.Height = 2160.f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT rect = {};
	rect.left = 0;
	rect.top = 0;
	rect.right = 3840.f;
	rect.bottom = 2160.f;
	cmdList->RSSetViewports(1, &viewport);
	cmdList->RSSetScissorRects(1, &rect);

	auto depthView = m_shadowDepth->GetDSV_CPU();
	m_shadowCamera->InitWithDirectionalLight(m_lightManager->GetDirectionalLight(), 25.f * 1.77777777778f, 25.f, 0.1f, 100.f);

	cmdList->ClearDepthStencilView(depthView, D3D12_CLEAR_FLAG_DEPTH, 1.f, 0, 0, nullptr);
	cmdList->SetPipelineState(m_PipelineStateObject);
	cmdList->SetGraphicsRootSignature(m_RootSignature);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->OMSetRenderTargets(0, nullptr, FALSE, &depthView);
	//cmdList->Depth
}

void RenderPassShadowMap::RunPass(uint8_t contextID)
{
	ID3D12GraphicsCommandList* cmdList = m_renderer->GetContextCmdList(contextID);

	m_shadowCameraCB->CPUData().viewMatrix = m_shadowCamera->GetShadowView();
	m_shadowCameraCB->CPUData().projectionMatrix = m_shadowCamera->GetShadowProjection();
	m_shadowCameraCB->CPUData().viewProjectionMatrix = m_shadowCamera->GetShadowViewProjection();
	m_shadowCameraCB->SendConstantDataToGPU();
	m_shadowCameraCB->SetAsInlineRootDescriptor(cmdList, 0);

	for (auto& mehes : m_meshes)
	{
		mehes->DrawMesh(true);
	}

	// todo: move flipping cb index to the end of the frame (it should be called one time after all the drawing)
	m_shadowCameraCB->FlipCBIndex();
}

void RenderPassShadowMap::EndPass(uint8_t contextID)
{
	ID3D12GraphicsCommandList* cmdList = m_renderer->GetContextCmdList(contextID);

	m_renderer->BindSwapchainToRTV();
}