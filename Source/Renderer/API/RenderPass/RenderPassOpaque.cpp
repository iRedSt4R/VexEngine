#include "RenderPasses.h"

#include "../D3D12/ShaderCompiler/DX12ShaderLibrary.h"

void RenderPassStaticOpaque::Create(DX12Renderer* renderer)
{
	m_renderer = renderer;
	m_device = renderer->GetD3D12Device();

	m_cameraCB = new ConstantBuffer<CameraCB>(m_device);

	// vertex layout
	D3D12_INPUT_ELEMENT_DESC inputLayout_PosCol[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.NumElements = sizeof(inputLayout_PosCol) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = inputLayout_PosCol;

	// vertex shader:
	m_vsShader = DX12ShaderLibrary::Get()->GetVertexShader(L"../../Source/Renderer/Shaders/2D/2d_vs_color.hlsl");
	m_psShader = DX12ShaderLibrary::Get()->GetPixelShader(L"../../Source/Renderer/Shaders/2D/2d_ps_color.hlsl");

	//Root siganature:
	// 1 Constant buffers:
	// 0: camera CB (view/projection)
	// 1: model CB (model) 
	// 2: dirLight CB
	// 3: descriptor table (size 1) x2 for srv textures
	// 5 shadowLight CB
	
	// cbv
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor;
	rootCBVDescriptor.RegisterSpace = 0;
	rootCBVDescriptor.ShaderRegister = 0;
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor2;
	rootCBVDescriptor2.RegisterSpace = 0;
	rootCBVDescriptor2.ShaderRegister = 1;
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor3;
	rootCBVDescriptor3.RegisterSpace = 0;
	rootCBVDescriptor3.ShaderRegister = 2;
	D3D12_ROOT_DESCRIPTOR rootCBVDescriptor4;
	rootCBVDescriptor4.RegisterSpace = 0;
	rootCBVDescriptor4.ShaderRegister = 3;

	// srv table
	D3D12_DESCRIPTOR_RANGE  descriptorTableRanges[1];
	descriptorTableRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorTableRanges[0].NumDescriptors = VEX_TEXTURE_2D_BINDLESS_TABLE_SIZE;
	descriptorTableRanges[0].BaseShaderRegister = 0; // tx in shader, t0 in this case
	descriptorTableRanges[0].RegisterSpace = 0;
	descriptorTableRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// cubeMpa table
	D3D12_DESCRIPTOR_RANGE  descriptorTableRanges1[1];
	descriptorTableRanges1[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorTableRanges1[0].NumDescriptors = VEX_TEXTURE_2D_BINDLESS_TABLE_SIZE;
	descriptorTableRanges1[0].BaseShaderRegister = 1; // tx in shader, t0 in this case
	descriptorTableRanges1[0].RegisterSpace = 1;
	descriptorTableRanges1[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable;
	descriptorTable.NumDescriptorRanges = _countof(descriptorTableRanges);
	descriptorTable.pDescriptorRanges = &descriptorTableRanges[0];

	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable2;
	descriptorTable2.NumDescriptorRanges = _countof(descriptorTableRanges1);
	descriptorTable2.pDescriptorRanges = &descriptorTableRanges1[0];

	// combine parameters
	D3D12_ROOT_PARAMETER  rootParameters[6];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor = rootCBVDescriptor;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].Descriptor = rootCBVDescriptor2;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[2].Descriptor = rootCBVDescriptor3;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[3].DescriptorTable = descriptorTable;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].Descriptor = rootCBVDescriptor4;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[5].DescriptorTable = descriptorTable2;
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// add sapler for texture
	D3D12_STATIC_SAMPLER_DESC m_samplerDesc[3];
	m_samplerDesc[0] = {};
	m_samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	m_samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[0].MipLODBias = 0;
	m_samplerDesc[0].MaxAnisotropy = 0;
	m_samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	m_samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	m_samplerDesc[0].MinLOD = 0.0f;
	m_samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	m_samplerDesc[0].ShaderRegister = 1;
	m_samplerDesc[0].RegisterSpace = 0;
	m_samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	m_samplerDesc[1] = {};
	m_samplerDesc[1].Filter = D3D12_FILTER_ANISOTROPIC;
	m_samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[1].MipLODBias = 0;
	m_samplerDesc[1].MaxAnisotropy = 16;
	m_samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	m_samplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	m_samplerDesc[1].MinLOD = 0.0f;
	m_samplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	m_samplerDesc[1].ShaderRegister = 0;
	m_samplerDesc[1].RegisterSpace = 0;
	m_samplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	m_samplerDesc[2] = {};
	m_samplerDesc[2].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	m_samplerDesc[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	m_samplerDesc[2].MipLODBias = 0;
	m_samplerDesc[2].MaxAnisotropy = 0;
	m_samplerDesc[2].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	m_samplerDesc[2].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	m_samplerDesc[2].MinLOD = 0.0f;
	m_samplerDesc[2].MaxLOD = D3D12_FLOAT32_MAX;
	m_samplerDesc[2].ShaderRegister = 2;
	m_samplerDesc[2].RegisterSpace = 0;
	m_samplerDesc[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = _countof(rootParameters);
	rootSignatureDesc.pParameters = rootParameters;
	rootSignatureDesc.NumStaticSamplers = _countof(m_samplerDesc);
	rootSignatureDesc.pStaticSamplers = m_samplerDesc;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	//rootSignatureDesc.Init(1, rootParameters,  nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ID3DBlob* signature;
	ID3DBlob* errorBlob;
	HRESULT grr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errorBlob);

	if (errorBlob)
	{
		auto error = (char*)errorBlob->GetBufferPointer();
		std::string errorMsg = error;
	}

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
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
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

void RenderPassStaticOpaque::BeginPass(uint8_t contextID)
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
	m_shadowDepth->ChangeState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, cmdList);

	DX12ResoruceAllocator::Get()->SetBindlessTexture2DHeap(3, cmdList);
	DX12ResoruceAllocator::Get()->SetBindlessTexture2DHeap(5, cmdList);
}

void RenderPassStaticOpaque::RunPass(uint8_t contextID)
{
	ID3D12GraphicsCommandList* cmdList = m_renderer->GetContextCmdList(contextID);

	m_cameraCB->CPUData().viewMatrix = m_camera->GetViewMatrix();
	m_cameraCB->CPUData().projectionMatrix = m_camera->GetProjectionMatrix();
	m_cameraCB->CPUData().worldCameraPosition = m_camera->GetCameraPosition();
	m_cameraCB->SendConstantDataToGPU();

	m_cameraCB->SetAsInlineRootDescriptor(cmdList, 0);
	m_lightManager->GetDirectionalLight()->SetAsInlineRootDescriptor(cmdList, 2);

	for (auto& mehes : m_meshes)
	{
		mehes->DrawMesh();
	}
}

void RenderPassStaticOpaque::EndPass(uint8_t contextID)
{
	ID3D12GraphicsCommandList* cmdList = m_renderer->GetContextCmdList(contextID);
}