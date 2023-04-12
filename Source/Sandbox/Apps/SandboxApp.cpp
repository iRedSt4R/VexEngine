#include "SandboxApp.h"
#include <Windows/DisplayManager.h>

void SandboxApp::Create(HINSTANCE hInstance, uint32_t height, uint32_t width)
{
	// WinApi App
	m_winApp = new Win64App;
	m_winApp->Init(hInstance, width, height);

	m_winApp->AddLoopCallbacks(std::bind(&SandboxApp::Begin, this), std::bind(&SandboxApp::Update, this), std::bind(&SandboxApp::End, this));
	DisplayManager::Get()->SetCurrentApp(m_winApp);

	// Init renderer
	m_renderer = DX12Renderer::Get();
	m_renderer->Create(width, height, 2, m_winApp->GetWindowHandle());

	// fps camera
	m_camera = new FPSCamera(width, height);
	m_camera->Create();

	MaterialFactory::Init(m_renderer->GetD3D12Device(), m_renderer->GetContextCmdList(0));

	// sponza mesh
	m_mesh = new Mesh(m_renderer->GetD3D12Device(), m_renderer->GetContextCmdList(0));
	//m_mesh->LoadMesh("sponza/Sponza.gltf", "sponza/");
	 m_mesh->LoadMesh("spheresRoughness.glb", "");
	//m_mesh->LoadBinaryMesh("Assets/sponza.vexmesh");
	//m_mesh->LoadBinaryMesh("Assets/sponaSerialized.vexmesh");
	//m_mesh->Serialize("TestSponza.vexmesh");

	//m_sphere = new Mesh(m_renderer->GetD3D12Device(), m_renderer->GetContextCmdList(0));
	//m_sphere->LoadMesh("sphere/sphere.glb", "sphere/");

	// light manager (with directional light)
	m_lightNamanger = LightManager::Get();
	m_dirLight = new DirectionalLight();
	m_dirLight->Create(m_renderer->GetD3D12Device(), XMFLOAT3(0.95f, 0.95f, -0.2f), XMFLOAT3(1.0f, 1.0f, 1.0f));
	m_lightNamanger->AddDirectionalLight(m_dirLight);

	// shadow framebuffer
	m_shadowDepthTexture = DX12ResoruceAllocator::Get()->AllocateDepthTexture2D(3840, 2160, DXGI_FORMAT_D32_FLOAT, true, false);
	m_dirLight->SetShadowTextureIndex(m_shadowDepthTexture->GetSRVIndexInsideHeap());

	// passes defs:
	RenderPassShadowMap* shadowPass = new RenderPassShadowMap();
	shadowPass->AddMesh(m_mesh);
	//shadowPass->AddMesh(m_sphere);
	shadowPass->AddLightManager(m_lightNamanger);
	shadowPass->AddDepthBuffer(m_shadowDepthTexture);
	shadowPass->AddFPSCamera(m_camera);
	shadowPass->Create(m_renderer);
	m_renderPasses.push_back(shadowPass);

	
	RenderPassCubeMapDraw* cubemapDrawRenderPass = new RenderPassCubeMapDraw();
	cubemapDrawRenderPass->AddCamera(m_camera);
	cubemapDrawRenderPass->Create(m_renderer);
	m_renderPasses.push_back(cubemapDrawRenderPass);

	RenderPassStaticOpaque* twoDPass = new RenderPassStaticOpaque();
	twoDPass->Create(m_renderer);
	twoDPass->AddMesh(m_mesh);
	//twoDPass->AddMesh(m_sphere);
	twoDPass->AddCamera(m_camera);
	twoDPass->SetLightManager(m_lightNamanger);
	twoDPass->AddShadowSRV(m_shadowDepthTexture);
	m_renderPasses.push_back(twoDPass);

	//Material* testMat = MaterialFactory::CreatePBRMaterialNoTextures(0.7f, 0.2f, XMFLOAT3(1.f, 1.f, 1.f));
	//Material* testMat = MaterialFactory::CreatePBRMaterialNoTextures(0.0f, 0.0f, XMFLOAT3(0.f, 0.f, 0.f));
	//testMat->Deserialize("testmat.vexmaterial");
	//testMat->Serialize("testmat.vexmaterial");
	//delete testMat;
	
	// From this call the m_WinApp will control the application flow from windows callbacks to Begin(), Update() and End()
	m_winApp->AppLoop();
}

void SandboxApp::Begin()
{
	m_renderer->BeginFrame();
}

void SandboxApp::Update()
{
	m_camera->Update(m_width, m_height, m_winApp->GetMouseDeltaX(), m_winApp->GetMouseDeltaY());

	for (auto& renderPass : m_renderPasses)
	{
		renderPass->BeginPass(0);
		renderPass->RunPass(0);
		renderPass->EndPass(0);
	}
}

void SandboxApp::End()
{
	m_renderer->EndFrame();
}

void SandboxApp::Shutdown()
{

}