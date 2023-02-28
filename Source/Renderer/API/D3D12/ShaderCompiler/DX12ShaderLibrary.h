#pragma once
#include "../../../Common/VexRenderCommon.h"
#include "DX12ShaderCompiler.h"

class DX12ShaderLibrary
{
public:
	DX12ShaderLibrary();
	~DX12ShaderLibrary();

	static DX12ShaderLibrary* Get()
	{
		if (s_instance == nullptr)
		{
			s_instance = new DX12ShaderLibrary();
		}

		return s_instance;
	}

	void CompileAndAddVertexShader(std::wstring shaderPath);
	void CompileAndAddPixelShader(std::wstring shaderPath);

	void UpdateVertexShaderBytecode(std::wstring shaderPath, D3D12_SHADER_BYTECODE newVSBytecode);
	void UpdatePixelShaderBytecode(std::wstring shaderPath, D3D12_SHADER_BYTECODE newPSBytecode);

	void AddVertexShader(std::wstring shaderPath, D3D12_SHADER_BYTECODE vsBytecode);
	void AddPixelShader(std::wstring shaderPath, D3D12_SHADER_BYTECODE vsBytecode);

	void SerializeAll(std::filesystem::path pathToSerialize);
	void SerializeVertexShader(std::filesystem::path pathToSerialize);
	void SerializePixelShader(std::filesystem::path pathToSerialize);

	__forceinline D3D12_SHADER_BYTECODE GetVertexShader(std::wstring shaderPath) { return m_vsShaders.at(shaderPath); }
	__forceinline D3D12_SHADER_BYTECODE GetPixelShader(std::wstring shaderPath) { return m_psShaders.at(shaderPath); }

	__forceinline uint32_t GetVertexShadersCount() { return m_vsShaders.size(); }
	__forceinline uint32_t GetPixelShadersCount() { return m_psShaders.size(); }
	__forceinline uint32_t GetAllShadersCount() { return GetVertexShadersCount() + GetPixelShadersCount(); }
private:
	inline static DX12ShaderLibrary* s_instance = nullptr;
	DX12ShaderCompiler* m_shaderCompiler;

	std::unordered_map<std::wstring, D3D12_SHADER_BYTECODE> m_vsShaders;
	std::unordered_map<std::wstring, D3D12_SHADER_BYTECODE> m_psShaders;
};