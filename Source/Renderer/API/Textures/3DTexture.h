#pragma once

#include "../D3D12/DX12Resource.h"
#include "../D3D12/Allocator/DX12DescriptorHeap.h"

class DX12Resource;

class TextureCube
{
public:
	TextureCube(ID3D12Device* device) { m_device = device; };
	~TextureCube();

	void CreateFromFile(ID3D12GraphicsCommandList* cmdList, std::wstring pathToTexture, bool bMarkAsSRGB = true)
	{
		m_textureResource = DX12ResoruceAllocator::Get()->AllocateTextureCubeFromFilepath(cmdList, pathToTexture, bMarkAsSRGB);
	};

	void SetAsGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, int rootParamIndex)
	{
		cmdList->SetGraphicsRootDescriptorTable(rootParamIndex, m_textureResource->GetSRV());
	}

	__forceinline DX12Resource* GetDX12Resource() { return m_textureResource; }

private:
	ID3D12Device* m_device;
	DX12Resource* m_textureResource = nullptr;
};