#pragma once

#include "../D3D12/DX12Resource.h"
#include "../D3D12/Allocator/DX12DescriptorHeap.h"

class DX12Resource;

class Texture2D
{
public:
	Texture2D(ID3D12Device* device) { m_device = device; };
	~Texture2D();

	void CreateFromFile(ID3D12GraphicsCommandList* cmdList, std::wstring pathToTexture, bool bMarkAsSRGB = true)
	{ 
		m_textureResource = DX12ResoruceAllocator::Get()->AllocateTexture2DFromFilepath(cmdList, pathToTexture, bMarkAsSRGB);
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