#pragma once

#include "../../Common/VexRenderCommon.h"
#include "ShaderCompiler/DX12ShaderCompiler.h"

class DX12ShaderBase
{
public:
	DX12ShaderBase() {};
	virtual ~DX12ShaderBase() {};

	__forceinline D3D12_SHADER_BYTECODE GetShaderByteCode() { return m_ShaderByteCode; }
	__forceinline ID3DBlob* GetShaderBlob() { return m_ShaderBlob; }
	__forceinline ID3DBlob* GetErrorBufPtr() { return m_ErrorBuff; }

protected:
	D3D12_SHADER_BYTECODE m_ShaderByteCode;
	ID3DBlob* m_ShaderBlob = nullptr;
	ID3DBlob* m_ErrorBuff = nullptr;
};

class DX12VertexShader : public DX12ShaderBase
{
public:
	DX12VertexShader();
	~DX12VertexShader();

	void CreateFromFile(LPCWSTR fileName, bool bDebug, std::string entryPointName = "main");
private:
};

class DX12PixelShader : public DX12ShaderBase
{
public:

	DX12PixelShader();
	~DX12PixelShader();

	void CreateFromFile(LPCWSTR fileName, bool bDebug, std::string entryPointName = "main");
private:

};
