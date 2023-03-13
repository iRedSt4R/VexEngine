#pragma once

#include "FPSCamera.h"
#include "ShadowCamera.h"

class CameraManager
{
public:
	CameraManager()
		:m_ActiveCamera(nullptr), m_PrevActiveCamera(nullptr)
	{

	};
	~CameraManager() {};

	static CameraManager* Get()
	{
		if (s_CameraManager == nullptr)
		{
			s_CameraManager = new CameraManager();
		}
		return s_CameraManager;
	}

	__forceinline void SetActiveCamera(CameraBase* camera) 
	{ 
		m_PrevActiveCamera = m_ActiveCamera;  
		m_ActiveCamera = camera;
	}

	__forceinline void SetDirLightShadowCamera(ShadowCamera* shadowCamera)
	{
		m_dirLightSadowCam = shadowCamera;
	}

	__forceinline CameraBase* GetActiveCamera() { return m_ActiveCamera; }
	__forceinline CameraBase* GetPrevActiveCamera() { return m_PrevActiveCamera; }

private:
	static CameraManager* s_CameraManager;

	CameraBase* m_ActiveCamera;
	CameraBase* m_PrevActiveCamera;

	ShadowCamera* m_dirLightSadowCam = nullptr;
};