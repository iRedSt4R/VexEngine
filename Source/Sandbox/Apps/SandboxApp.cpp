#include "SandboxApp.h"
#include <Windows/DisplayManager.h>

void SandboxApp::Create(HINSTANCE hInstance, uint32_t height, uint32_t width)
{
	m_winApp = new Win64App;
	m_winApp->Init(hInstance, width, height);

	m_winApp->AddLoopCallbacks(std::bind(&SandboxApp::Begin, this), std::bind(&SandboxApp::Update, this), std::bind(&SandboxApp::End, this));
	DisplayManager::Get()->SetCurrentApp(m_winApp);

	m_renderer = DX12Renderer::Get();
	m_renderer->Create(width, height, 2, m_winApp->GetWindowHandle());

	m_camera = new FPSCamera(width, height);
	m_camera->Create();

	m_mesh = new Mesh(m_renderer->GetD3D12Device(), m_renderer->GetContextCmdList(0));
	m_mesh->LoadMesh("sponza/Sponza.gltf", "sponza/");

	m_lightNamanger = LightManager::Get();
	m_dirLight = new DirectionalLight();
	m_dirLight->Create(m_renderer->GetD3D12Device(), XMFLOAT3(0.5f, 0.5f, -0.8f), XMFLOAT3(1.f, 1.f, 0.8f));
	m_lightNamanger->AddDirectionalLight(m_dirLight);

	RenderPassStaticOpaque* twoDPass = new RenderPassStaticOpaque();
	twoDPass->Create(m_renderer->GetD3D12Device());
	twoDPass->AddMesh(m_mesh);
	twoDPass->AddCamera(m_camera);
	twoDPass->SetLightManager(m_lightNamanger);
	m_renderPasses.push_back(twoDPass);


	// From this call the m_WinApp will control the application flow from windows callbacks
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
		renderPass->BeginPass(m_renderer->GetContextCmdList(0));
		renderPass->RunPass(m_renderer->GetContextCmdList(0));
		renderPass->EndPass(m_renderer->GetContextCmdList(0));
	}
}

void SandboxApp::End()
{
	m_renderer->EndFrame();
}

void SandboxApp::Shutdown()
{

}