#pragma once

#include "../../Common/VexRenderCommon.h"
using namespace DirectX;

class CameraBase
{
public:
	CameraBase() {};
	virtual ~CameraBase() {};

	virtual void Create() = 0;
	virtual void Update(uint32_t width, uint32_t height, int* mouseDeltaX, int* mouseDeltaY) = 0;

	__forceinline XMFLOAT4X4 GetInvViewProjMatrix() { return m_InvViewProjection; }
	__forceinline XMFLOAT4X4 GetViewMatrix() { return m_ViewMatrix; }
	__forceinline XMFLOAT4X4 GetProjectionMatrix() { return m_ProjectionMatrix; }

	__forceinline XMFLOAT4 GetCameraPosition() { return m_CameraPositon; }
	__forceinline XMFLOAT4 GetCameraUp() { return m_CameraUp; }
	__forceinline XMFLOAT4 GetCameraTarget() { return m_CameraTarget; }

	__forceinline float GetCameraFov() { return m_Fov; }
	__forceinline float GetCameraAspectRation() { return m_AspectRatio; }

protected:
	XMFLOAT4 m_CameraPositon;
	XMFLOAT4 m_CameraUp;
	XMFLOAT4 m_CameraTarget;

	float m_Fov;
	float m_AspectRatio;

	XMFLOAT4X4 m_ViewMatrix;
	XMFLOAT4X4 m_InvViewMatrix;
	XMFLOAT4X4 m_ProjectionMatrix;
	XMFLOAT4X4 m_InvViewProjection;
};