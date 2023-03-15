#include "DX12IndexedVertexBuffer.h"
#include "d3dx12.h"

DX12IndexedVertexBuffer::DX12IndexedVertexBuffer(ID3D12Device* device)
	:m_Device(device)
{

}

DX12IndexedVertexBuffer::~DX12IndexedVertexBuffer()
{

}

void DX12IndexedVertexBuffer::Create(ID3D12GraphicsCommandList* cmdList, int vertexBufferByteSize, int vertexBufferStride, int indexCount, void* vbData, void* ibData)
{
	m_vertexBufferByteSize = vertexBufferByteSize;
	m_vertexBufferByteStride = vertexBufferByteSize;
	m_indexCount = indexCount;

	// ------------------------------------------------------------------- VERTEX BUFFER ------------------------------------------------------------------- //
	// Copy index data from CPU to GPU index buffer through upload heap
	// Create Default Heap
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferByteSize);
	auto heapPropertiesDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto heapPropertiesUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	m_Device->CreateCommittedResource(
		&heapPropertiesDefault,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&m_VertexBuffer));

	m_VertexBuffer->SetName(L"Vertex Buffer Resource Heap");

	// Create Upload Heap
	auto uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferByteSize);
	ID3D12Resource* vBufferUploadHeap;
	m_Device->CreateCommittedResource(
		&heapPropertiesUpload,
		D3D12_HEAP_FLAG_NONE,
		&uploadBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vBufferUploadHeap));
	vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap");

	// Upload Heap -> Default Heap
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(vbData);
	vertexData.RowPitch = vertexBufferByteSize;
	vertexData.SlicePitch = vertexBufferByteSize;

	m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
	m_VertexBufferView.StrideInBytes = vertexBufferStride;
	m_VertexBufferView.SizeInBytes = vertexBufferByteSize;

	UpdateSubresources(cmdList, m_VertexBuffer, vBufferUploadHeap, 0, 0, 1, &vertexData);

	auto barrierToExe = CD3DX12_RESOURCE_BARRIER::Transition(m_VertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	cmdList->ResourceBarrier(1, &barrierToExe);

	// ------------------------------------------------------------------- INDEX BUFFER ------------------------------------------------------------------- //
	if (indexCount > 0)
	{
		m_bVertexBufferOnly = false;
		// Copy index data from CPU to GPU index buffer through upload heap
		// Create Default Heap
		auto indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexCount * sizeof(uint32_t));
		m_Device->CreateCommittedResource(
			&heapPropertiesDefault,
			D3D12_HEAP_FLAG_NONE,
			&indexBufferDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_IndexBuffer));
	
		m_IndexBuffer->SetName(L"Index Buffer Resource Heap");
	
		// Create Upload Heap
		auto indexUploadDesc = CD3DX12_RESOURCE_DESC::Buffer(indexCount * sizeof(uint32_t));
		ID3D12Resource* vBufferUploadHeap2;
		m_Device->CreateCommittedResource(
			&heapPropertiesUpload,
			D3D12_HEAP_FLAG_NONE,
			&indexUploadDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vBufferUploadHeap2));
		vBufferUploadHeap2->SetName(L"Index Buffer Upload Resource Heap");
	
		// Upload Heap -> Default Heap
		D3D12_SUBRESOURCE_DATA indexData = {};
		indexData.pData = reinterpret_cast<BYTE*>(ibData);
		indexData.RowPitch = indexCount * sizeof(uint32_t);
		indexData.SlicePitch = indexCount * sizeof(uint32_t);
	
		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_IndexBufferView.SizeInBytes = indexCount * sizeof(uint32_t);
	
		UpdateSubresources(cmdList, m_IndexBuffer, vBufferUploadHeap2, 0, 0, 1, &indexData);
	
		barrierToExe = CD3DX12_RESOURCE_BARRIER::Transition(m_IndexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		cmdList->ResourceBarrier(1, &barrierToExe);
	}
}
