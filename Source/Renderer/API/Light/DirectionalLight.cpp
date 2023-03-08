#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{

}

DirectionalLight::~DirectionalLight()
{

}

void DirectionalLight::Create(ID3D12Device* device, XMFLOAT3 lightDirection, XMFLOAT3 lightColor)
{
	m_device = device;
	m_constantBuffer = new ConstantBuffer<CBData>(m_device);

	m_constantBuffer->CPUData().lightDirection = lightDirection;
	m_constantBuffer->CPUData().lightColor = lightColor;
	m_constantBuffer->SendConstantDataToGPU();
}
