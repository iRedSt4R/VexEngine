#pragma once

#include "../../Common/VexRenderCommon.h"
#include "DirectionalLight.h"

class LightManager
{
public:
	LightManager() {};
	~LightManager() {};

	static LightManager* Get()
	{
		if (s_manager == nullptr)
			s_manager = new LightManager();

		return s_manager;
	}

	void AddDirectionalLight(DirectionalLight* dirLight) { m_directionalLight = dirLight; };
	void AddDirectionalLight(XMFLOAT3 lightDirection, XMFLOAT3 lightColor);
	//void AddPointLight()
	//void AddSpotLight()
	//void AddSkyLight()
	//void AddAmbientLight()

	__forceinline DirectionalLight* GetDirectionalLight() { return m_directionalLight; }
private:
	DirectionalLight* m_directionalLight = nullptr;

	inline static LightManager* s_manager = nullptr;
};