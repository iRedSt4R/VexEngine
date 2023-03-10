#pragma once
#include <string>
#include <vector>

#include "../../Common/VexRenderCommon.h"
#include "../D3D12/DX12Renderer.h"

using namespace DirectX;

class IRenderPass
{
public:
	// Lunched once on engine start
	virtual void Create(DX12Renderer* renderer) = 0;

	//Called every frame, BeginPass->RunPass->EndPass
	virtual void BeginPass(uint8_t contextID) = 0;
	virtual void RunPass(uint8_t contextID) = 0;
	virtual void EndPass(uint8_t contextID) = 0;

	virtual std::string GetPassName() = 0;
};

class RenderPassBase : public IRenderPass
{
public:
	virtual std::string GetPassName() override { return m_PassName; }
	__forceinline void SetPassName(std::string passName) { m_PassName = passName; }

protected:

	std::string m_PassName;
	ID3D12Device* m_device = nullptr;
	DX12Renderer* m_renderer = nullptr;

	HRESULT hr;
};
