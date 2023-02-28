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

	m_triangleVertices[0] = {XMFLOAT3(0.0f, 0.5f, 0.5f), XMFLOAT4(0.7f, 0.7f, 0.7f, 1.f)};
	m_triangleVertices[1] = { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(0.2f, 0.1f, 0.8f, 1.f) };
	m_triangleVertices[2] = { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(0.5f, 0.2f, 0.4f, 1.f) };

	m_triangleIndices[0] = 0;
	m_triangleIndices[1] = 1;
	m_triangleIndices[2] = 2;

	m_vertexBuffer = m_renderer->CreateIndexedVertexBuffer(sizeof(m_triangleVertices), sizeof(VertexP3C4), 3, &m_triangleVertices[0], &m_triangleIndices[0]);

	// From this call the m_WinApp will control the application flow from windows callbacks
	m_winApp->AppLoop();
}

void SandboxApp::Begin()
{
	m_renderer->BeginFrame();
}

void SandboxApp::Update()
{
	m_renderer->SetIndexedVertexBuffer(0, m_vertexBuffer);
	m_renderer->DrawIndexed(0, 3);
}

void SandboxApp::End()
{
	m_renderer->EndFrame();
}

void SandboxApp::Shutdown()
{

}