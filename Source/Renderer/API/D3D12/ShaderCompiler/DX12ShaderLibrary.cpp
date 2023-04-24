#include "DX12ShaderLibrary.h"

DX12ShaderLibrary::DX12ShaderLibrary()
{
	m_shaderCompiler = new DX12ShaderCompiler();

	// Precompile shaders at the beginning

	// opaque geometry draw
	CompileAndAddVertexShader(L"../../Source/Renderer/Shaders/2D/2d_vs_color.hlsl");
	CompileAndAddPixelShader(L"../../Source/Renderer/Shaders/2D/2d_ps_color.hlsl");

	// directional light shadow map generation
	CompileAndAddVertexShader(L"../../Source/Renderer/Shaders/2D/vs_depth_pass.hlsl");

	// cubemap draw shaders
	CompileAndAddVertexShader(L"../../Source/Renderer/Shaders/Skybox/vs_skybox_draw.hlsl");
	CompileAndAddPixelShader(L"../../Source/Renderer/Shaders/Skybox/ps_skybox_draw.hlsl");

	CompileAndAddVertexShader(L"../../Source/Renderer/Shaders/PBR/vs_gen_irradiance.hlsl");
	CompileAndAddPixelShader(L"../../Source/Renderer/Shaders/PBR/ps_gen_irradiance.hlsl");

}

DX12ShaderLibrary::~DX12ShaderLibrary()
{
	delete m_shaderCompiler;
}

void DX12ShaderLibrary::CompileAndAddVertexShader(std::wstring shaderPath)
{
	auto vsBytecode = m_shaderCompiler->CompileVertexShader(shaderPath);
	m_vsShaders.insert({ shaderPath , vsBytecode });
}

void DX12ShaderLibrary::CompileAndAddPixelShader(std::wstring shaderPath)
{
	auto psBytecode = m_shaderCompiler->CompilePixelShader(shaderPath);
	m_psShaders.insert({ shaderPath , psBytecode });
}