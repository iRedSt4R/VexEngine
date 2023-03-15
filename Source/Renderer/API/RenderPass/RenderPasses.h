#pragma once
#include "../../Common/VexRenderCommon.h"
#include "RenderPassBase.h"
#include <dxcapi.h>
#include "../D3D12/ShaderCompiler/DX12ShaderLibrary.h"
#include "../D3D12/DX12IndexedVertexBuffer.h"
#include "../D3D12/DX12ConstantBuffer.h"
#include "../Camera/FPSCamera.h"
#include "../Camera/ShadowCamera.h"
#include "../Textures/2DTexture.h"
#include "../Meshes/Mesh.h"
#include "../Light/LightManager.h"
#include "../Meshes/Skybox.h"

//Passes:
// basic opaque pass for static geometry
class RenderPassStaticOpaque : public RenderPassBase
{
public:
	void Create(DX12Renderer* renderer) override final;
	void BeginPass(uint8_t contextID) override final;
	void RunPass(uint8_t contextID) override final;
	void EndPass(uint8_t contextID) override final;

	void AddMesh(Mesh* mesh) { m_meshes.push_back(mesh); }

	void AddCamera(FPSCamera* camera) { m_camera = camera; }
	void AddTexture(Texture2D* texture) { m_texture = texture; }
	void AddShadowSRV(DX12Resource* depthSRV) { m_shadowDepth = depthSRV; }
	void SetLightManager(LightManager* lightManager) { m_lightManager = lightManager; }

private:
	// Root signature and PSO unique to this pass
	ID3D12RootSignature* m_RootSignature;
	ID3D12PipelineState* m_PipelineStateObject;

	//std::vector<DX12IndexedVertexBuffer*> m_vertexBuffers = {};
	std::vector<Mesh*> m_meshes = {};

	// Shaders
	D3D12_SHADER_BYTECODE m_vsShader;
	D3D12_SHADER_BYTECODE m_psShader;

	FPSCamera* m_camera = nullptr;
	Texture2D* m_texture = nullptr;
	ConstantBuffer<CameraCB>* m_cameraCB;
	LightManager* m_lightManager = nullptr;

	// depth SRV for shadows
	DX12Resource* m_shadowDepth = nullptr;

};

// pass for shadow map generation:
class RenderPassShadowMap : public RenderPassBase
{
public:
	void Create(DX12Renderer* renderer) override final;
	void BeginPass(uint8_t contextID) override final;
	void RunPass(uint8_t contextID) override final;
	void EndPass(uint8_t contextID) override final;

	void AddMesh(Mesh* mesh) { m_meshes.push_back(mesh); }
	void AddDepthBuffer(DX12Resource* shadowDepthRes) { m_shadowDepth = shadowDepthRes; }
	void AddLightManager(LightManager* lightManager) { m_lightManager = lightManager; }
	void AddFPSCamera(FPSCamera* fpsCamera) { m_camera = fpsCamera; }

private:
	// Root signature and PSO unique to this pass
	ID3D12RootSignature* m_RootSignature;
	ID3D12PipelineState* m_PipelineStateObject;

	// vs
	D3D12_SHADER_BYTECODE m_vsShader;

	// meshes
	std::vector<Mesh*> m_meshes = {};

	// shadow camera
	ConstantBuffer<ShadowCameraCB>* m_shadowCameraCB;
	ShadowCamera* m_shadowCamera = nullptr;
	FPSCamera* m_camera = nullptr;

	// depth RT
	DX12Resource* m_shadowDepth = nullptr;

	// light data for shadows
	LightManager* m_lightManager = nullptr;
};

// RenderPass for rendering cubemap (non-hdr)
class RenderPassCubeMapDraw : public RenderPassBase
{
public:
	void Create(DX12Renderer* renderer) override final;
	void BeginPass(uint8_t contextID) override final;
	void RunPass(uint8_t contextID) override final;
	void EndPass(uint8_t contextID) override final;

	void AddCamera(FPSCamera* camera) { m_camera = camera; }

private:
	// Root signature and PSO unique to this pass
	ID3D12RootSignature* m_RootSignature;
	ID3D12PipelineState* m_PipelineStateObject;

	// Shaders
	D3D12_SHADER_BYTECODE m_vsShader;
	D3D12_SHADER_BYTECODE m_psShader;

	Skybox* m_skybox = nullptr;
	FPSCamera* m_camera = nullptr;
	

};