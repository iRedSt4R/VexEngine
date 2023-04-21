#include "ShadowCamera.h"
#include "../Light/LightManager.h"

#define DIR_CAMERA_HEIGHT 25.f

void ShadowCamera::InitWithDirectionalLight(DirectionalLight* dirLight, float orthoWidth, float orthoHeight, float nearFlane, float farPlane)
{
	m_shadowView = XMFLOAT4X4();
	m_shadowProj = XMFLOAT4X4();
	m_shadowVP = XMFLOAT4X4();

	// ortho projection
	XMMATRIX tmpMat = XMMatrixOrthographicLH(orthoWidth, orthoHeight, nearFlane, farPlane);
	tmpMat = XMMatrixTranspose(tmpMat);
	XMStoreFloat4x4(&m_shadowProj, tmpMat);

	// view
	//XMFLOAT4 camPosTemp = XMFLOAT4(dirLight->GetLightDirection().x, dirLight->GetLightDirection().y, dirLight->GetLightDirection().z, 1.f);
	//XMFLOAT3 temp = XMFLOAT3(dirLight->GetLightDirection().x, dirLight->GetLightDirection().y, dirLight->GetLightDirection().z);

	XMFLOAT4 m_CameraPositon = XMFLOAT4(dirLight->GetLightDirection().x * DIR_CAMERA_HEIGHT, dirLight->GetLightDirection().y * DIR_CAMERA_HEIGHT, dirLight->GetLightDirection().z * DIR_CAMERA_HEIGHT, 0.0f);
	//XMFLOAT4 m_CameraPositon = XMFLOAT4(-2.f, 4.f, -1.f, 0.0f);
	XMFLOAT4 m_CameraTarget = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 m_CameraUp = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

	XMVECTOR camTarget = XMLoadFloat4(&m_CameraPositon);
	XMVECTOR camUp = XMLoadFloat4(&m_CameraTarget);
	XMVECTOR camPos = XMLoadFloat4(&m_CameraUp);
	XMMATRIX shadowViewTemp = XMMatrixLookAtLH(camTarget, camUp, camPos);
	shadowViewTemp = XMMatrixTranspose(shadowViewTemp);
	XMStoreFloat4x4(&m_shadowView, shadowViewTemp);

	LightManager::Get()->GetDirectionalLight()->GetDirectionalLightCB()->CPUData().dirLightViewMatrix = m_shadowView;
	LightManager::Get()->GetDirectionalLight()->GetDirectionalLightCB()->CPUData().dirLightProjMatrix = m_shadowProj;
	LightManager::Get()->GetDirectionalLight()->GetDirectionalLightCB()->SendConstantDataToGPU();

}
