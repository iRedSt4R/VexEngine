#include "RenderPasses.h"

void RenderPassGenIrradiance::Create(DX12Renderer* renderer)
{
	m_renderer = renderer;
	m_device = renderer->GetD3D12Device();

	m_vertices = new float[12];
	m_vertices[0] = -1.0f;
	m_vertices[1] = 1.0f;
	m_vertices[2] = 0.f;
	m_vertices[3] = 0.f;

	m_vertices[4] = 3.0f;
	m_vertices[5] = 1.0f;
	m_vertices[6] = 2.0f;
	m_vertices[7] = 0.f;

	m_vertices[8] = -1.0f;
	m_vertices[9] = -3.0f;
	m_vertices[10] = 0.0f;
	m_vertices[11] = 2.0f;

	m_fullscreenTriangle = new DX12IndexedVertexBuffer(m_device);
	m_fullscreenTriangle->Create(renderer->GetContextCmdList(0), sizeof(float) * 12, sizeof(float) * 4, 0, m_vertices, nullptr);

	D3D12_INPUT_ELEMENT_DESC inputLayout_PosCol[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = sizeof(inputLayout_PosCol) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout_PosCol;

	// vertex shader:
	m_vsShader = DX12ShaderLibrary::Get()->GetVertexShader(L"../../Source/Renderer/Shaders/PBR/vs_gen_irradiance.hlsl");
	m_psShader = DX12ShaderLibrary::Get()->GetPixelShader(L"../../Source/Renderer/Shaders/PBR/ps_gen_irradiance.hlsl");

	// cubemap CB
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;

	D3D12_ROOT_CONSTANTS rootConstants = {};
	rootConstants.Num32BitValues = 1;	
	rootConstants.RegisterSpace = 0;
	rootConstants.ShaderRegister = 1;

	D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1];
	descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorTableRanges[0].NumDescriptors = VEX_TEXTURE_2D_BINDLESS_TABLE_SIZE;
	descriptorTableRanges[0].BaseShaderRegister = 0; // tx in shader, t0 in this case
	descriptorTableRanges[0].RegisterSpace = 0;
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];

	D3D12_ROOT_PARAMETER  rootParameters[3];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = rootCBVDescriptor;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable = descriptorTable;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameters[2].Constants = rootConstants;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC m_samplerDesc[1];
	m_samplerDesc[0] = {};
	m_samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	m_samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	m_samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	m_samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	m_samplerDesc[0].MipLODBias = 0;
	m_samplerDesc[0].MaxAnisotropy = 1.f;
	m_samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	m_samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	m_samplerDesc[0].MinLOD = 0.0f;
	m_samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	m_samplerDesc[0].ShaderRegister = 0;
	m_samplerDesc[0].RegisterSpace = 0;
	m_samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = _countof(rootParameters);
	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumStaticSamplers = _countof(m_samplerDesc);
	rootSignatureDesc.pStaticSamplers = m_samplerDesc;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	ID3DBlob* signature;
	ID3DBlob* errorBlob;
	HRESULT grr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBlob);

	if (errorBlob)
	{
		auto error = (char*)errorBlob->GetBufferPointer();
		std::string errorMsg = error;
	}

	m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));

	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.pRootSignature = m_RootSignature;
	psoDesc.VS = m_vsShader;
	psoDesc.PS = m_psShader;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
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

void RenderPassGenIrradiance::BeginPass(uint8_t contextID)
{
	auto cmdList = m_renderer->GetContextCmdList(contextID);

	m_irradianceMap->ChangeState(D3D12_RESOURCE_STATE_RENDER_TARGET, cmdList);


	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = m_irradianceMap->GetWidth();
	viewport.Height = m_irradianceMap->GetHeight();
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT rect = {};
	rect.left = 0;
	rect.top = 0;
	rect.right = m_irradianceMap->GetWidth();
	rect.bottom = m_irradianceMap->GetHeight();
	cmdList->RSSetViewports(1, &viewport);
	cmdList->RSSetScissorRects(1, &rect);
	cmdList->SetPipelineState(m_PipelineStateObject);
	cmdList->SetGraphicsRootSignature(m_RootSignature);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void RenderPassGenIrradiance::RunPass(uint8_t contextID)
{
	auto cmdList = m_renderer->GetContextCmdList(contextID);

	m_fullscreenTriangle->Set(cmdList);
	
	DX12ResoruceAllocator::Get()->SetBindlessTexture2DHeap(1, cmdList);


	m_skybox->BindConstantBuffer(cmdList, 0);

	for (uint32_t sliceID = 0; sliceID < 6; sliceID++)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE irrHandle;
		irrHandle.ptr = m_irradianceMap->GetRTV_CPU().ptr + (m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * sliceID);
		cmdList->OMSetRenderTargets(1, &irrHandle, NULL, nullptr);

		//m_skybox->GetConstantBuffer()->CPUData().sliceIndex = sliceID;
		//m_skybox->GetConstantBuffer()->SendConstantDataToGPU();
		cmdList->SetGraphicsRoot32BitConstant(2, sliceID, 0);
		cmdList->DrawInstanced(3, 1, 0, 0);
	}
}

void RenderPassGenIrradiance::EndPass(uint8_t contextID)
{
	auto cmdList = m_renderer->GetContextCmdList(contextID);

	m_irradianceMap->ChangeState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, cmdList);
	m_renderer->BindSwapchainToRTV();
}

RenderPassGenIrradiance::~RenderPassGenIrradiance()
{
	delete m_vertices;
	delete m_fullscreenTriangle;
}
