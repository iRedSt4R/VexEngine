#pragma once

#include "../../Common/VexRenderCommon.h"
#include "RenderComponentBase.h"
#include "../Meshes/Mesh.h"
#include "../Meshes/Material.h"

struct StaticMeshRenderComponentHeader
{
	
};

class StaticMeshRenderComponent : public RenderComponentBase<StaticMeshRenderComponent>
{
public:

	StaticMeshRenderComponent();
	~StaticMeshRenderComponent();

	//void CreateFromBinary(std::filesystem::path path); // load .vexsmesh
	//void SetMaterial(Material* material) { m_material = material; };

	//__forceinline 
	//void Create(std::filesystem:::path meshBlob);
private:
	//Mesh* m_mesh = nullptr;
	//Material* m_material = nullptr;
};