#pragma once

#include "../../Common/VexRenderCommon.h"
#include "../D3D12/DX12ConstantBuffer.h"


class DirectionalLight
{
	struct CBData
	{
		XMFLOAT3 lightDirection;
		float pad0;
		XMFLOAT3 lightColor;
		float pad1;
	};

public:
	DirectionalLight();
	~DirectionalLight();

	void Create(ID3D12Device* device, XMFLOAT3 lightDirection, XMFLOAT3 lightColor);

	void SetAsInlineRootDescriptor(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndex)
	{
		m_constantBuffer->SetAsInlineRootDescriptor(cmdList, rootParamIndex);
	}

	void EndDraw() { m_constantBuffer->FlipCBIndex(); };

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