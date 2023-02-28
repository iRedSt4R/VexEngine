#include "DX12ShaderLibrary.h"

DX12ShaderLibrary::DX12ShaderLibrary()
{
	m_shaderCompiler = new DX12ShaderCompiler();

	// Precompile shaders at the beginning
	CompileAndAddVertexShader(L"../../Source/Renderer/Shaders/2D/2d_vs_color.hlsl");
	CompileAndAddPixelShader(L"../../Source/Renderer/Shaders/2D/2d_ps_color.hlsl");

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