#pragma once
#include <Common\VexRenderCommon.h>
#include "MeshHeaders.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class SimpleStaticMesh
{
public:
	SimpleStaticMesh();
	~SimpleStaticMesh();

	void LoadAndFillHeader(const aiScene* scene, int meshIndex);
	std::ostream& operator<< (std::ostream& stream) {};

private:
	float* m_vertices = nullptr;
	uint32_t m_vertexCount;
	uint32_t* m_Indicies = nullptr;
	int m_IndexCount = 0;
	int m_faceCount = 0;

	SimpleStaticMeshHeader m_meshHeader = {};
};

class StaticMesh
{

};