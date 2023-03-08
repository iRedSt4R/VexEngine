#pragma once

#include "../../Common/VexRenderCommon.h"
#include "../Light/DirectionalLight.h"
#include "../D3D12/DX12ConstantBuffer.h"

class ShadowCamera
{
public:
	ShadowCamera() {};
	~ShadowCamera() {};

	void InitWithDirectionalLight(DirectionalLight* dirLight, float orthoWidth, float orthoHeight, float nearFlane, float farPlane);

	__forceinline XMFLOAT4X4 GetShadowView() { return m_shadowView; }
	__forceinline XMFLOAT4X4 GetShadowProjection() { return m_shadowProj; }
	__forceinline XMFLOAT4X4 GetShadowViewProjection() { return m_shadowVP; }
private:
	XMFLOAT4X4 m_shadowView = {};
	XMFLOAT4X4 m_shadowProj = {};
	XMFLOAT4X4 m_shadowVP = {};
};