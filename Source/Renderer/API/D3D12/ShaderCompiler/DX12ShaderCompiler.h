#pragma once

#include "../../../Common/VexRenderCommon.h"
#include "dxcapi.h"

class DX12ShaderCompiler
{
public:
	DX12ShaderCompiler();
	~DX12ShaderCompiler();

	D3D12_SHADER_BYTECODE CompileVertexShader(std::filesystem::path vsPath, bool useCache = true, bool createCache = true);
	D3D12_SHADER_BYTECODE CompilePixelShader(std::filesystem::path psPath, bool useCache = true, bool createCache = true);

private:
	IDxcCompiler* m_Compiler;
	IDxcUtils* m_Utils;

};