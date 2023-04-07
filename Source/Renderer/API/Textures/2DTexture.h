#pragma once

#include "../D3D12/DX12Resource.h"
#include "../D3D12/Allocator/DX12DescriptorHeap.h"

class DX12Resource;

class Texture2D
{
public:
	Texture2D(ID3D12Device* device) { m_device = device; };
	~Texture2D() {};

	void CreateFromFile(ID3D12GraphicsCommandList* cmdList, std::wstring pathToTexture, bool bMarkAsSRGB = true)
	{ 
		m_textureResource = DX12ResoruceAllocator::Get()->AllocateTexture2DFromFilepath(cmdList, pathToTexture, bMarkAsSRGB);
	};

	void CreateFromBinary(ID3D12GraphicsCommandList* cmdList, void* blobMemory, size_t blobByteSize, bool bMarkAsSRGB = true)
	{
		m_textureResource = DX12ResoruceAllocator::Get()->LoadTexture2DFromBinary(cmdList, blobMemory, blobByteSize, bMarkAsSRGB);
	}

	static DirectX::Blob* LoadFromFile(std::wstring pathToTexture, bool bMarkAsSRGB = true)
	{
		return DX12ResoruceAllocator::Get()->LoadTexture2DFromFilepath(pathToTexture, bMarkAsSRGB);
	}

	void SetAsGraphicsRootDescriptorTable(ID3D12GraphicsCommandList* cmdList, int rootParamIndex)
	{
		cmdList->SetGraphicsRootDescriptorTable(rootParamIndex, m_textureResource->GetSRV());
	}

	__forceinline DX12Resource* GetDX12Resource() { return m_textureResource; }

private:
	ID3D12Device* m_device;

	DX12Resource* m_textureResource = nullptr;
};


class Texture2DCache
{
public:
	Texture2DCache() {};
	~Texture2DCache() {};

	void Init(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) { m_device = device; m_cmdList = cmdList; };

	static Texture2DCache* Get()
	{
		if (s_instance == nullptr)
		{
			s_instance = new Texture2DCache();
		}
		return s_instance;
	}

	Texture2D* CreateTexture2D(std::wstring pathToTexture, bool bMarkAsSRGB = true);
	__forceinline uint32_t GetCacheSize() { return m_textureCache.size(); }

private:
	inline static Texture2DCache* s_instance = nullptr;
	ID3D12Device* m_device = nullptr;
	ID3D12GraphicsCommandList* m_cmdList = nullptr;
	std::unordered_map<std::wstring, Texture2D*> m_textureCache = {};
};
