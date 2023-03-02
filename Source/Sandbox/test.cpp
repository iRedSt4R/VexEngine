#include <VexRenderer.h>
#include "Apps/SandboxApp.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

//DX12RenderContext* renderCtx = nullptr;
static SandboxApp* m_app;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// create console
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	auto curPath = std::filesystem::current_path();

	m_app = new SandboxApp();
	m_app->Create(hInstance, 600, 900);

	//renderCtx = new DX12RenderContext();
	//renderCtx->Create();

	//DX12ShaderCompiler* shaderCompiler = new DX12ShaderCompiler();
	//shaderCompiler->CompileVertexShader(L"vs_test.hlsl");

	return 0;
}