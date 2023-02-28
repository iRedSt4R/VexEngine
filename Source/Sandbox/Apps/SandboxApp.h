#include <Core/ApplicationBase.h>
#include <API/D3D12/DX12Renderer.h>

class SandboxApp : public IApplication
{
public:
	SandboxApp() {};
	virtual ~SandboxApp() {};

	void Create(HINSTANCE hInstance, uint32_t height, uint32_t width) override;
	void Begin() override;
	void Update() override;
	void End() override;
	void Shutdown() override;

	__forceinline Win64App* GetWin64App() { return m_winApp; };

private:
	Win64App* m_winApp = nullptr;
	DX12Renderer* m_renderer = nullptr;

	DX12IndexedVertexBuffer* m_vertexBuffer;
};