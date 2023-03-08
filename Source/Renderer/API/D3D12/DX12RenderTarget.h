#include "../../Common/VexRenderCommon.h"
#include "DX12Resource.h"

class DX12RenderTarget
{
public:
	DX12RenderTarget();
	~DX12RenderTarget();

	void Init(ID3D12Device* device, uint8_t colorBufferCount, bool withDepthStencilBuffer, bool bInitAsUAV = false);

	void ClearAll();
	void ClearAllColorBuffers();
	void ClearDepthBuffer();
	void ClearColorBuffer(uint8_t clearIndices);

	// colorSetFlag - which color buffers to bind
	void SetAsRenderTarget(ID3D12CommandList* cmdList, uint8_t colorSetFlag);
	//void SetAsSRV(ID3D12CommandList* cmdList, ...)
	//void CopyToRenderTarget(DX12Resource* newRenderTarget, bool bFlushCurrent);
	//void Flush();

	__forceinline size_t GetColorBuffersCount() { return m_ColorBuffers.size(); }
	__forceinline DX12Resource* GetColorBuffer(uint8_t colorBufferIndex) { return m_ColorBuffers[colorBufferIndex]; }
	__forceinline DX12Resource* GetDepthStencilBuffer() { return m_DepthStencilBuffer; }

private:
	ID3D12Device* m_device;
	std::vector<DX12Resource*> m_ColorBuffers;
	DX12Resource* m_DepthStencilBuffer = nullptr;
};