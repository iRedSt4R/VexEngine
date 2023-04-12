#pragma once
#include "../../Common/VexRenderCommon.h"
#include "Mesh.h"

class VexLevel
{
public:
	VexLevel() {};
	~VexLevel() {};

	void Serialize(std::filesystem::path pathToSerialize);
	void AddToLevel(Mesh* meshToAdd);
private:
	std::vector<Mesh*> m_meshes = {};
	std::string m_levelName = {};
};