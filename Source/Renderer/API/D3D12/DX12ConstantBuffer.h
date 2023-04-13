#pragma once

#include "../../Common/VexRenderCommon.h"
#include "Allocator/DX12DescriptorHeap.h"

using namespace DirectX;

struct CBFloat
{
	float multiplier;
	float multiplier2;
	float multiplier3;
	float multiplier4;
};

struct CBModelViewProjection
{
	XMFLOAT4X4  model;
	XMFLOAT4X4  view;
	XMFLOAT4X4  projection;
};

struct CameraCB
{
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	XMFLOAT4X4 invViewProjMatrix;
	XMFLOAT4 worldCameraPosition;
};

struct ShadowCameraCB
{
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	XMFLOAT4X4 viewProjectionMatrix;
};

struct CBSceneModel
{
	XMFLOAT4X4 modelMatrix;
};

struct CBStaticMeshData
{
	XMFLOAT4X4 meshWorldMatrix; // mesh position/rotation/scale
	XMFLOAT3 color;
	float roughness = 1.f;
	float metalness = 0.f;
	uint32_t albedoIndexInHeap = 0;
	uint32_t normalIndexInHeap = 0;
	uint32_t roughnessIndexInHeap = 0;
	uint32_t metallicIndexInHeap = 0;
	uint32_t bHaveAlbedoTex = false;
	uint32_t bHaveNormalTex = false;
	uint32_t bHaveRoughnessTex = false;
	uint32_t bHaveMetallicTex = false;
};

struct CBCubeMapData
{
	XMFLOAT4X4 WorldViewProjection;
	uint32_t textureCubeIndex = 0;
};

class ConstantBufferBase
{
public:
	virtual void FlipCBIndex() = 0;
};


class ConstantBufferFlipper
{
public:
	static void AddConstantBufferToFlip(ConstantBufferBase* cbToFlip) { s_buffersToFlip.push_back(cbToFlip); }

	static void FlipAllConstantBuffers()
	{
		for (auto& buffer : s_buffersToFlip)
		{
			buffer->FlipCBIndex();
		}
	}
private:
	inline static std::vector<ConstantBufferBase*> s_buffersToFlip = {};
};

template <typename CBStructType>
class ConstantBuffer : public ConstantBufferBase
{
public:
	// Creates constant buffer with already allocated descriptor heap for it
	ConstantBuffer(ID3D12Device* device)
	{
		m_currentCBIndex = 0;
		ZeroMemory(&m_CBData, sizeof(CBStructType));

		DX12ResoruceAllocator* allocator = DX12ResoruceAllocator::Get();
		for (int id = 0; id < VEX_RENDER_BACK_BUFFER_COUNT; id++)
		{
			m_ConstantBuffer[id] = allocator->AllocateConstantBuffer(sizeof(CBStructType));

			// Get address of CB already in ctor with Map function (address is valid till destruction)
			D3D12_RANGE readRange;
			readRange.Begin = 0;
			readRange.End = 0;
			m_ConstantBuffer[id]->Map(0, &readRange, reinterpret_cast<void**>(&m_ConstantBufferMapPtr[id]));
		}

		ConstantBufferFlipper::AddConstantBufferToFlip(this);
	};

	~ConstantBuffer() 
	{
	};

	__forceinline void SendConstantDataToGPU()
	{
		memcpy(m_ConstantBufferMapPtr[m_currentCBIndex], &m_CBData, sizeof(CBStructType));
	}

	__forceinline void SetAsInlineRootDescriptor(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndex)
	{
		cmdList->SetGraphicsRootConstantBufferView(rootParamIndex, m_ConstantBuffer[m_currentCBIndex]->GetGPUVirtualAddress());
	}

	__forceinline void SetAsComputeConstantBufferView(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndex)
	{
		cmdList->SetComputeRootConstantBufferView(rootParamIndex, m_ConstantBuffer[m_currentCBIndex]->GetGPUVirtualAddress());
	}

	__forceinline virtual void FlipCBIndex() override final
	{
#if VEX_RENDER_BACK_BUFFER_COUNT == 2
		m_currentCBIndex = (m_currentCBIndex == 0 ? 1 : 0);
#else
		__debugbreak();
#endif
	};

	__forceinline ID3D12Resource* GetCBResource() { return m_ConstantBuffer[m_currentCBIndex]; }
	__forceinline CBStructType& CPUData() { return m_CBData; }
private:
	// D3D12
	ID3D12Resource* m_ConstantBuffer[VEX_RENDER_BACK_BUFFER_COUNT]; // Constant Buffer
	UINT8* m_ConstantBufferMapPtr[VEX_RENDER_BACK_BUFFER_COUNT]; // Mapped ptr for updating constant buffer, valid till resource destruction
	uint8_t m_currentCBIndex; // Current index of free resource, other may be still in use
	CBStructType m_CBData; // Actual struct holding constant buffer data
};
