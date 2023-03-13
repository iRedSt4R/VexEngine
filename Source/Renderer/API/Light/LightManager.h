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
	void AddDirectionalLightShadowTexture(DX12Resource* shadowResource) { m_dirLightShadowTexture = shadowResource; }
	void AddDirectionalLight(XMFLOAT3 lightDirection, XMFLOAT3 lightColor);

	
	//void AddPointLight()
	//void AddSpotLight()
	//void AddSkyLight()
	//void AddAmbientLight()

	__forceinline DirectionalLight* GetDirectionalLight() { return m_directionalLight; }
	__forceinline DX12Resource* GetDirectionalLightShadowTexture() { return m_dirLightShadowTexture; }
private:
	DirectionalLight* m_directionalLight = nullptr;
	DX12Resource* m_dirLightShadowTexture = nullptr;

	inline static LightManager* s_manager = nullptr;
};