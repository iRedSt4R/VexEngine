#pragma once
#include "RenderPassBase.h"
#include "../../Common/VexRenderCommon.h"
#include "../D3D12/ShaderCompiler/DX12ShaderLibrary.h"
#include <dxcapi.h>
#include "../D3D12/DX12IndexedVertexBuffer.h"
#include "../Camera/FPSCamera.h"
#include "../D3D12/DX12ConstantBuffer.h"
#include "../Textures/2DTexture.h"
#include "../Meshes/Mesh.h"

//Passes:
class RenderPass2D : public RenderPassBase
{
public:
	void Create(ID3D12Device* device) override final;
	void BeginPass(ID3D12GraphicsCommandList* cmdList) override final;
	void RunPass(ID3D12GraphicsCommandList* cmdList) override final;
	void EndPass(ID3D12GraphicsCommandList* cmdList) override final;

	void AddMesh(Mesh* mesh) { m_meshes.push_back(mesh); }

	void AddCamera(FPSCamera* camera) { m_camera = camera; }
	void AddTexture(Texture2D* texture) { m_texture = texture; }

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

};
