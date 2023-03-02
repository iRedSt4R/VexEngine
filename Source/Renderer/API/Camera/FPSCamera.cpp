#include "FPSCamera.h"

FPSCamera::FPSCamera(uint32_t width, uint32_t height)
{
	// build projection
	XMMATRIX tmpMat = XMMatrixPerspectiveFovLH(60.0f * (3.14f / 180.0f), (float)width / (float)height, 0.1f, 1000.0f);
	tmpMat = XMMatrixTranspose(tmpMat);
	XMStoreFloat4x4(&m_ProjectionMatrix, tmpMat);

	// set starting camera state
	m_CameraPositon = XMFLOAT4(0.0f, 2.0f, -4.0f, 0.0f);
	m_CameraTarget = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_CameraUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

	DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camYaw = 0.0f;
	camPitch = 0.0f;

	GetCursorPos(&cursorOldPos);
	GetCursorPos(&cursorCurPos);
}

FPSCamera::~FPSCamera()
{

}

void FPSCamera::Create()
{

}

void FPSCamera::Update(uint32_t width, uint32_t height, int* mouseDeltaX, int* mouseDeltaY)
{
	XMVECTOR camTarget = XMLoadFloat4(&m_CameraTarget);
	XMVECTOR camUp = XMLoadFloat4(&m_CameraUp);
	XMVECTOR camPos = XMLoadFloat4(&m_CameraPositon);

	POINT mouseDelta;
	mouseDelta.x = (LONG)*mouseDeltaX;
	mouseDelta.y = (LONG)*mouseDeltaY;

	*mouseDeltaX = 0;
	*mouseDeltaY = 0;

	// Read inputs
	// Read inputs
	if (GetKeyState(0x41) & 0x8000) //left
	{
		camPos -= 0.001f * camRight;
	}
	if (GetKeyState(0x44) & 0x8000) //right
	{
		camPos += 0.001f * camRight;
	}
	if (GetKeyState(0x57) & 0x8000) //forward
	{
		camPos += 0.001f * camForward;
	}
	if (GetKeyState(0x53) & 0x8000) //backward
	{
		camPos -= 0.001f * camForward;
	}


	camYaw += mouseDelta.x * 0.001f;
	camPitch += mouseDelta.y * 0.001f;

	XMMATRIX camView;
	XMMATRIX camProj;
	XMMATRIX camInvView;
	XMMATRIX camViewProj;
	XMMATRIX camInvViewProj;

	camProj = XMMatrixPerspectiveFovLH(60.0f * (3.14f / 180.0f), (float)width / (float)height, 0.1f, 1000.0f);

	camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
	camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camTarget = XMVector3Normalize(camTarget);

	camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
	camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
	camUp = XMVector3Cross(camForward, camRight);

	camPos += moveLeftRight * camRight;
	camPos += moveBackForward * camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	camTarget = camPos + camTarget;

	camView = XMMatrixLookAtLH(camPos, camTarget, camUp);
	camViewProj = camView * camProj;
	camInvViewProj = XMMatrixInverse(nullptr, camViewProj);

	camView = XMMatrixTranspose(camView);
	camInvViewProj = XMMatrixTranspose(camInvViewProj);


	XMStoreFloat4x4(&m_ViewMatrix, camView);
	XMStoreFloat4x4(&m_InvViewProjection, camInvViewProj);

	XMStoreFloat4(&m_CameraPositon, camPos);
	XMStoreFloat4(&m_CameraUp, camUp);
	XMStoreFloat4(&m_CameraTarget, camTarget);
}
