#include <Core/ApplicationBase.h>
#include <API/D3D12/DX12Renderer.h>
#include <API/Camera/FPSCamera.h>
#include <API/Textures/2DTexture.h>
#include <API/Light/LightManager.h>
#include <API/Meshes/Material.h>

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
	LightManager* m_lightNamanger = nullptr;
	DirectionalLight* m_dirLight = nullptr;

	std::vector<IRenderPass*> m_renderPasses = {};

	// RTs
	DX12Resource* m_shadowDepthTexture = nullptr;

	//window data:
	uint32_t m_height = 0;
	uint32_t m_width = 0;;

	FPSCamera* m_camera = nullptr;
	Mesh* m_mesh = nullptr;
	Mesh* m_sphere = nullptr;
};