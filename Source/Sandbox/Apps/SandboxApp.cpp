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

	m_vertexBuffer = new DX12IndexedVertexBuffer(m_renderer->GetD3D12Device());
	//m_vertexBuffer->Create()

	// From this call the m_WinApp will control the application flow from windows callbacks
	m_winApp->AppLoop();
}

void SandboxApp::Begin()
{
	m_renderer->BeginFrame();
}

void SandboxApp::Update()
{

}

void SandboxApp::End()
{
	m_renderer->EndFrame();
}

void SandboxApp::Shutdown()
{

}