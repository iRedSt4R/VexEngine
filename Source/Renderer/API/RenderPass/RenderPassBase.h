#pragma once
#include <string>
#include <vector>

//#include "../../Common/EngineCommon.h"
#include "../D3D12/DX12IndexedVertexBuffer.h"

using namespace DirectX;

class IRenderPass
{
public:
	// Lunched once on engine start
	virtual void Create(ID3D12Device* device) = 0;

	//Called every frame, BeginPass->RunPass->EndPass
	virtual void BeginPass(ID3D12GraphicsCommandList* cmdList) = 0;
	virtual void RunPass(ID3D12GraphicsCommandList* cmdList) = 0;
	virtual void EndPass(ID3D12GraphicsCommandList* cmdList) = 0;

	virtual std::string GetPassName() = 0;
};

class RenderPassBase : public IRenderPass
{
public:
	virtual std::string GetPassName() override { return m_PassName; }
	__forceinline void SetPassName(std::string passName) { m_PassName = passName; }

	//__forceinline void AddGeometryPrimitive(IPrimitiveGeometry* primitiveGeometry) { m_PrimitiveGeometryList.push_back(primitiveGeometry); };

protected:
	std::vector<DX12IndexedVertexBuffer*> m_vertexBuffers;

	std::string m_PassName;
	ID3D12Device* m_device;

	HRESULT hr;
};
