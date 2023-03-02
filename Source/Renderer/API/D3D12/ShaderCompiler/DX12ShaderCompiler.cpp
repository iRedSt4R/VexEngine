#include "DX12ShaderCompiler.h"

DX12ShaderCompiler::DX12ShaderCompiler()
{
	DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&m_Utils));
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&m_Compiler));
}

DX12ShaderCompiler::~DX12ShaderCompiler()
{

}

D3D12_SHADER_BYTECODE DX12ShaderCompiler::CompileVertexShader(std::filesystem::path vsPath, bool useCache, bool createCache)
{
	D3D12_SHADER_BYTECODE returnBytecode;

	// Load shader source
	uint32_t codePage = CP_UTF8;
	IDxcBlobEncoding* sourceBlob;
	m_Utils->LoadFile(vsPath.c_str(), &codePage, &sourceBlob);

	// Compile shader
	std::vector<LPCWSTR> arguments;
	arguments.push_back(L"-no-warnings");

	IDxcOperationResult* result;
	m_Compiler->Compile(
		sourceBlob,
		vsPath.c_str(),
		L"vs_main", // pEntryPoint
		L"vs_6_1", // pTargetProfile
		arguments.data(), arguments.size(),
		NULL, 0,
		NULL,
		&result);

	HRESULT hr;
	if (result->GetStatus(&hr))
	{
		IDxcBlobEncoding* errorsBlob;
		result->GetErrorBuffer(&errorsBlob);
		if (errorsBlob)
		{
			std::cout << (L"Compilation failed with errors:\n%hs\n",
				(const char*)errorsBlob->GetBufferPointer()) << std::endl;
		}
	}

	IDxcBlob* shaderCode;
	result->GetResult(&shaderCode);

	returnBytecode.pShaderBytecode = shaderCode->GetBufferPointer();
	returnBytecode.BytecodeLength = shaderCode->GetBufferSize();

	SAFE_RELEASE(sourceBlob);

	return returnBytecode;
}

D3D12_SHADER_BYTECODE DX12ShaderCompiler::CompilePixelShader(std::filesystem::path psPath, bool useCache, bool createCache)
{
	D3D12_SHADER_BYTECODE returnBytecode;

	// Load shader source
	uint32_t codePage = CP_UTF8;
	IDxcBlobEncoding* sourceBlob;
	m_Utils->LoadFile(psPath.c_str(), &codePage, &sourceBlob);

	// Compiler defines
	std::vector<LPCWSTR> arguments;
	arguments.push_back(L"-no-warnings");

	// Compile shader
	IDxcOperationResult* result;
	m_Compiler->Compile(
		sourceBlob,
		psPath.c_str(),
		L"ps_main", // pEntryPoint
		L"ps_6_1", // pTargetProfile
		arguments.data(), arguments.size(),
		NULL, 0,
		NULL,
		&result);

	HRESULT hr;
	if (result->GetStatus(&hr))
	{
		IDxcBlobEncoding* errorsBlob;
		result->GetErrorBuffer(&errorsBlob);
		if (errorsBlob)
		{
			std::cout << (L"Compilation failed with errors:\n%hs\n",
				(const char*)errorsBlob->GetBufferPointer()) << std::endl;
		}
	}

	IDxcBlob* shaderCode;
	result->GetResult(&shaderCode);

	returnBytecode.pShaderBytecode = shaderCode->GetBufferPointer();
	returnBytecode.BytecodeLength = shaderCode->GetBufferSize();

	SAFE_RELEASE(sourceBlob);

	return returnBytecode;
}
