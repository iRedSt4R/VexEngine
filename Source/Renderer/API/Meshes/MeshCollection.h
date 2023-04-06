#pragma once
#include "../../Common/VexRenderCommon.h"
#include "Mesh.h"

struct MeshCollectionHeader
{
	uint32_t meshCount = 0;
};

class MeshCollection
{
public:
	MeshCollection();
	~MeshCollection();

	void Serialize(std::filesystem::path pathToSerialize);
	void AddToBatch(Mesh* meshToAdd);
private:
	std::vector<Mesh*> m_meshes = {};
};