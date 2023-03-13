#pragma once

#include "../../Common/VexRenderCommon.h"
#include "../D3D12/DX12ConstantBuffer.h"


class DirectionalLight
{
public:
	struct CBData
	{
		XMFLOAT4X4 dirLightViewMatrix;
		XMFLOAT4X4 dirLightProjMatrix;
		XMFLOAT3 lightDirection;
		uint32_t shadowMapIndexInsideHeap;
		XMFLOAT3 lightColor;
	};

public:
	DirectionalLight();
	~DirectionalLight();

	void Create(ID3D12Device* device, XMFLOAT3 lightDirection, XMFLOAT3 lightColor);
	void SetShadowTextureIndex(uint32_t shadowTextureIndex) 
	{ 
		m_constantBuffer->CPUData().shadowMapIndexInsideHeap = shadowTextureIndex;
		m_constantBuffer->SendConstantDataToGPU();
	}

	void SetAsInlineRootDescriptor(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndex)
	{
		m_constantBuffer->SetAsInlineRootDescriptor(cmdList, rootParamIndex);
	}

	void EndDraw() {};

	__forceinline void SetLightDirection(XMFLOAT3 lightDirection) { m_constantBuffer->CPUData().lightDirection = lightDirection;}
	__forceinline void SetLightColor(XMFLOAT3 lightColor) { m_constantBuffer->CPUData().lightColor = lightColor; }
	__forceinline void SendDataToGPU() { m_constantBuffer->SendConstantDataToGPU(); }

	__forceinline ConstantBuffer<CBData>* GetDirectionalLightCB() { return m_constantBuffer; }
	__forceinline const XMFLOAT3& GetLightDirection() { return m_constantBuffer->CPUData().lightDirection; }
	__forceinline const XMFLOAT3& GetLightColor() { return m_constantBuffer->CPUData().lightColor; }
private:
	ID3D12Device* m_device = nullptr;
	ConstantBuffer<CBData>* m_constantBuffer = nullptr;

};