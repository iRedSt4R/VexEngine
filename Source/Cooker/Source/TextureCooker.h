#pragma once
#include <API/D3D12/DX12Renderer.h>


class TextureCooker
{
public:
	TextureCooker() {};
	~TextureCooker() {};

	static void CookGLTFMeshToEngineFormat(std::filesystem::path pathToMesh)
	{

	}

private:
	inline static DX12Renderer* s_renderer = nullptr;
};