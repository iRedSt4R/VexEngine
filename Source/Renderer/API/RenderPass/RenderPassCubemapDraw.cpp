#include "RenderPasses.h"


void RenderPassCubeMapDraw::Create(DX12Renderer* renderer)
{
	m_renderer = renderer;
	m_device = renderer->GetD3D12Device();

	m_skybox = new Skybox(m_device);
	m_skybox->CreateSkyboxSphere(renderer->GetContextCmdList(0), L"Assets/skymaps/skymap.dds", 10, 10);

	D3D12_INPUT_ELEMENT_DESC inputLayout_PosCol[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = sizeof(inputLayout_PosCol) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout_PosCol;

	// vertex shader:
	m_vsShader = DX12ShaderLibrary::Get()->GetVertexShader(L"../../Source/Renderer/Shaders/Skybox/vs_skybox_draw.hlsl");
	m_psShader = DX12ShaderLibrary::Get()->GetPixelShader(L"../../Source/Renderer/Shaders/Skybox/ps_skybox_draw.hlsl");

	// Root signature:
	// 0: cubemap CB
	// 1: bindless srv table (used as textureCube inside hlsl, so be careful not to access anything other then textureCube from this shader)

	// cubemap CB
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;

	// bindless SRV table (as textureCube)
	D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1];
	descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorTableRanges[0].NumDescriptors = VEX_TEXTURE_2D_BINDLESS_TABLE_SIZE;
	descriptorTableRanges[0].BaseShaderRegister = 0; // tx in shader, t0 in this case
	descriptorTableRanges[0].RegisterSpace = 0;
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];

	// root params
	D3D12_ROOT_PARAMETER  rootParameters[2];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = rootCBVDescriptor;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[1].DescriptorTable = descriptorTable;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC m_samplerDesc[1];
	m_samplerDesc[0] = {};
	m_samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	m_samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[0].MipLODBias = 0;
	m_samplerDesc[0].MaxAnisotropy = 0;
	m_samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
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
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;

	ID3DBlob* signature;
	ID3DBlob* errorBlob;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBlob);

	m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));

	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1;

	D3D12_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.MultisampleEnable = FALSE;
	rasterDesc.AntialiasedLineEnable = FALSE;
	rasterDesc.ForcedSampleCount = 0;
	rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_DEPTH_STENCIL_DESC depthDesc = {};
	depthDesc.DepthEnable = FALSE;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.pRootSignature = m_RootSignature;
	psoDesc.VS = m_vsShader;
	psoDesc.PS = m_psShader;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	psoDesc.SampleDesc = sampleDesc;
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = rasterDesc;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // todo: not needed in cubemap drawing, disable it for better performance
	psoDesc.DepthStencilState = depthDesc; // todo: not needed in cubemap drawing, disable it for better performance
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.NumRenderTargets = 1;

	// create the PSO
	HRESULT hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineStateObject));
}

void RenderPassCubeMapDraw::BeginPass(uint8_t contextID)
{
	ID3D12GraphicsCommandList* cmdList = m_renderer->GetContextCmdList(contextID);

	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 1920.f;
	viewport.Height = 1080.f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	D3D12_RECT rect = {};
	rect.left = 0;
	rect.top = 0;
	rect.right = 1920.f;
	rect.bottom = 1080.f;
	cmdList->RSSetViewports(1, &viewport);
	cmdList->RSSetScissorRects(1, &rect);

	cmdList->SetPipelineState(m_PipelineStateObject);
	cmdList->SetGraphicsRootSignature(m_RootSignature);
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DX12ResoruceAllocator::Get()->SetBindlessTexture2DHeap(1, cmdList);
}

void RenderPassCubeMapDraw::RunPass(uint8_t contextID)
{
	m_skybox->Update(m_camera);
	m_skybox->Draw(m_renderer->GetContextCmdList(contextID), 0);
}

void RenderPassCubeMapDraw::EndPass(uint8_t contextID)
{

}
