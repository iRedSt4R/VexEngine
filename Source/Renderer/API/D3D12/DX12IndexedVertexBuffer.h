#pragma once
#include <d3d12.h>

/// <summary>
/// The same buffer can be used for normal and ray tracing pipeline
/// </summary>
class DX12IndexedVertexBuffer
{
	//friend class SimpleMesh;
	//friend class Mesh;

public:
	DX12IndexedVertexBuffer(ID3D12Device* device);
	~DX12IndexedVertexBuffer();

	void Create(ID3D12GraphicsCommandList* cmdList, int vertexBufferByteSize, int vertexBufferStride, int indexCount, void* vbData, void* ibData);

	__forceinline ID3D12Resource* GetVertexBufferResource() { return m_VertexBuffer; }
	__forceinline ID3D12Resource* GetIndexBufferResource() { return m_IndexBuffer; }
	__forceinline const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return m_VertexBufferView; }
	__forceinline const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return m_IndexBufferView; }
	//__forceinline const D3D12_RAYTRACING_GEOMETRY_DESC& GetRayTracingGeometryDesc() { return m_RTVertexBuffer; }
	//__forceinline D3D12_GPU_DESCRIPTOR_HANDLE GetVertexBufferSRVHandle() { return m_VertexBufferSRV_GPU; }
	//__forceinline D3D12_GPU_DESCRIPTOR_HANDLE GetIndexBufferSRVHandle() { return m_IndexBufferSRV_GPU; }

	void Set(ID3D12GraphicsCommandList* cmdList, UINT startSlot = 0, UINT numViews = 1)
	{
		cmdList->IASetVertexBuffers(startSlot, numViews, &m_VertexBufferView);
		cmdList->IASetIndexBuffer(&m_IndexBufferView);
	}

private:
	ID3D12Device* m_Device = nullptr;
	ID3D12Resource* m_VertexBuffer = nullptr;
	ID3D12Resource* m_IndexBuffer = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView {};
	D3D12_INDEX_BUFFER_VIEW m_IndexBufferView {};

	// Used to create accellaration structure
	//D3D12_RAYTRACING_GEOMETRY_DESC m_RTVertexBuffer;

	// Index buffer as SRV (RawByteBuffer)
	//D3D12_CPU_DESCRIPTOR_HANDLE m_IndexBufferSRV_CPU;
	//D3D12_GPU_DESCRIPTOR_HANDLE m_IndexBufferSRV_GPU;

	// Vertex buffer as SRV (StructuredBuffer)
	//D3D12_CPU_DESCRIPTOR_HANDLE m_VertexBufferSRV_CPU;
	//D3D12_GPU_DESCRIPTOR_HANDLE m_VertexBufferSRV_GPU;
};