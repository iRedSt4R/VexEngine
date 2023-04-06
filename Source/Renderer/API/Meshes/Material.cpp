#include "Material.h"


void Material::Serialize(std::filesystem::path pathToSerialize)
{
	//m_materialHeader
	std::ofstream meshBinary(pathToSerialize, std::ios::binary);
	meshBinary.write((char*)&m_materialHeader, sizeof(PBRMaterialHeader));
}

void Material::Deserialize(std::filesystem::path pathToDeserialize)
{
	auto fileSize = std::filesystem::file_size(pathToDeserialize);
	std::ifstream meshBinary(pathToDeserialize, std::ios::in | std::ios::binary);
	if (meshBinary)
		__debugbreak();
	else
		__debugbreak();
	//std::ifstream meshBinary2("testmat.vexmaterial", std::ios::in);
	char* data = new char[fileSize];
	meshBinary.read(data, fileSize);

	PBRMaterialHeader* header = reinterpret_cast<PBRMaterialHeader*>(data);

	m_roughness = header->roughness;
	m_metalness = header->metalness;
	m_color = XMFLOAT3(header->colorR, header->colorG, header->colorB);
	UpdateGPUData();
}

Material* MaterialFactory::CreatePBRMaterialNoTextures(float roughness, float metallness, XMFLOAT3 color)
{
	Material* newMaterial = new Material(s_device, s_cmdList0);
	newMaterial->CreatePBRNoTexture(roughness, metallness, color);

	return newMaterial;
}
