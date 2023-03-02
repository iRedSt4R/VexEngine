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

	m_triangleVertices[0] = {XMFLOAT3(0.0f, 0.5f, 0.5f), XMFLOAT2(0.5f, 0.f)};
	m_triangleVertices[1] = { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(1.f, 0.f) };
	m_triangleVertices[2] = { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.f, 1.f) };

	m_triangleIndices[0] = 0;
	m_triangleIndices[1] = 1;
	m_triangleIndices[2] = 2;

	//m_vertexBuffer = m_renderer->CreateIndexedVertexBuffer(sizeof(m_triangleVertices), sizeof(VertexP3T2), 3, &m_triangleVertices[0], &m_triangleIndices[0]);

	m_camera = new FPSCamera(width, height);
	m_camera->Create();

	m_texture = new Texture2D(m_renderer->GetD3D12Device());
	m_texture->CreateFromFile(m_renderer->GetContextCmdList(0), L"darude.jpg");

	m_mesh = new Mesh(m_renderer->GetD3D12Device(), m_renderer->GetContextCmdList(0));
	m_mesh->LoadMesh("sponza/Sponza.gltf", "sponza/");

	RenderPass2D* twoDPass = new RenderPass2D();
	twoDPass->Create(m_renderer->GetD3D12Device());
	twoDPass->AddMesh(m_mesh);
	twoDPass->AddCamera(m_camera);
	twoDPass->AddTexture(m_texture);
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