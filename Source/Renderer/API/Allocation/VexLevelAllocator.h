#pragma once

#include "../D3D12/Allocator/DX12DescriptorHeap.h"
#include "../D3D12/Allocator/D3D12MemAlloc.h"

class VexLevelAllocator
{
public:
	VexLevelAllocator();
	~VexLevelAllocator();

	void AllocateMap();
	void SwitchLevels();


	void Init();
	void Init(uint64_t allocatorByteSize);
	

private:

	D3D12MA::Allocator* m_Allocator = nullptr;
};