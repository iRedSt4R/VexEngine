#pragma once 
#include <API/D3D12/DX12Device.h>


// StaticMeshHeaderBlob binary layout:
// SimpleStaticMeshHeaders... | 1st SimpleStaticMeshBlob | 2nd SimpleStaticMeshBlob | 3rd SimpleStaticMeshBlob | ...
#pragma pack(push, 1)
struct StaticMeshHeader
{
	uint32_t simpleMeshHeaderCount = 0;
};
#pragma pack(pop)

// SimpleStaticMeshBlob binary layout:
// SimpleStaticMeshHeader | vertexData | indexData | albedoDDS | normalDDS | roughnessDDS | metallicDDS |
#pragma pack(push, 1)
struct SimpleStaticMeshHeader
{
	uint32_t blobByteSize = 0;
	uint32_t vertexByteSize = 0;
	uint32_t indexByteSize = 0;
	bool bHaveAlbedo = 0;
	bool bHaveNormal = 0;
	bool bHaveRoughness = 0;
	bool bHaveMetallic = 0;
	uint32_t albedoByteSize = 0;
	uint32_t normalByteSize = 0;
	uint32_t roughnessByteSize = 0;
	uint32_t metallicByteSize = 0;
	bool bHaveRoughnessMetallicCombined = 0;
};
#pragma pack(pop)