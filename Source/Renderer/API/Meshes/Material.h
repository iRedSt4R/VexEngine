#pragma once

#include "../../Common/VexRenderCommon.h"
#include "../D3D12/DX12ConstantBuffer.h"
#include "../Textures/2DTexture.h"

#pragma pack(push, 1)
struct PBRMaterialHeader
{
	uint32_t binaryByteSize = 0;
	float roughness = 0;
	float metalness = 0;

	uint8_t bHaveAlbedoTexture = 0;
	uint32_t albedoTextureOffset = 0;
	uint32_t albedoTextureByteSize = 0;

	uint8_t bHaveNormalTexture = 0;
	uint32_t normalTextureOffset = 0;
	uint32_t normalTextureByteSize = 0;

	uint8_t bHaveRoughnessTexture = 0;
	uint32_t roughnessTextureOffset = 0;
	uint32_t roughnessTextureByteSize = 0;

	uint8_t bHaveMetallicTexture = 0;
	uint32_t metallicTextureOffset = 0;
	uint32_t metallicTextureByteSize = 0;

	float colorR = 0;
	float colorG = 0;
	float colorB = 0;
};
#pragma pack(pop)

struct PBRMaterialCBData
{
	uint32_t roughness = 0;
	uint32_t metalness = 0;
	uint32_t bHaveAlbedoTexture = 0;
	uint32_t bHaveNormalTexture = 0;
	uint32_t bHaveRoughnessTexture = 0;
	uint32_t bHaveMetallicTexture = 0;
	XMFLOAT3 color;
};

class Material
{
public:
	//using PBRMaterialConstantBuffer = ConstantBuffer<PBRMaterialCBData>;

	Material(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) 
	{ 
		m_device = device; 
		m_cmdList = cmdList; 
		m_meshConstantBuffer = new ConstantBuffer<PBRMaterialCBData>(device);
	}
	~Material() 
	{
		delete m_meshConstantBuffer;

		if (m_albedoTexture)
			delete m_albedoTexture;
		if (m_normalTexture)
			delete m_normalTexture;
		if (m_roughnessTexture)
			delete m_roughnessTexture;
		if (m_metallicTexture)
			delete m_metallicTexture;
	};

	void UpdateGPUData() 
	{
		m_meshConstantBuffer->CPUData().roughness = m_roughness;
		m_meshConstantBuffer->CPUData().metalness = m_metalness;
		m_meshConstantBuffer->CPUData().color = m_color;
		m_meshConstantBuffer->SendConstantDataToGPU(); 
	}

	void CreatePBRNoTexture(float roughness, float metalness, XMFLOAT3 color)
	{
		m_roughness = roughness;
		m_metalness = metalness;
		m_color = color;

		m_materialHeader.roughness = roughness;
		m_materialHeader.metalness = metalness;
		m_materialHeader.colorR = color.x;
		m_materialHeader.colorG = color.y;
		m_materialHeader.colorB = color.z;

		UpdateGPUData();
	}

	__forceinline ConstantBuffer<PBRMaterialCBData>* GetPBRMaterialConstantBuffer() { return m_meshConstantBuffer; }
	void Serialize(std::filesystem::path pathToSerialize);
	void Deserialize(std::filesystem::path pathToDeserialize);
private:
	ID3D12Device* m_device = nullptr;
	ID3D12GraphicsCommandList* m_cmdList = nullptr;

	float m_roughness = 0.f;
	float m_metalness = 0.f;
	XMFLOAT3 m_color = {};

	Texture2D* m_albedoTexture = nullptr;
	Texture2D* m_normalTexture = nullptr;
	Texture2D* m_roughnessTexture = nullptr;
	Texture2D* m_metallicTexture = nullptr;

	ConstantBuffer<PBRMaterialCBData>* m_meshConstantBuffer = nullptr;
	PBRMaterialHeader m_materialHeader = {};
};

class MaterialFactory
{
public:
	static void Init(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList) { s_device = device; s_cmdList0 = cmdList; };

	static Material* CreatePBRMaterialNoTextures(float roughness, float metallness, XMFLOAT3 color);
	static Material* CreatePBRMaterialWithTextures(std::wstring albedoPath, std::wstring normalPath, std::wstring roughnessPath, std::wstring metallicPath);
	static Material* CreatePBRMaterialWithTextures(Texture2D* albedoTexture, Texture2D* normalTexture, Texture2D* roughnessTexture, Texture2D* metallicTexture);

	inline static ID3D12Device* s_device = nullptr;
	inline static ID3D12GraphicsCommandList* s_cmdList0 = nullptr;
};