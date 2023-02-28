#include "WinApp64.h"
#include "DisplayManager.h"

#include <iostream>
#include <string>
#include <chrono>

//LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Win64App* currentApp = DisplayManager::Get()->GetCurrentApp();

	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			currentApp->SetAppRunningState(false);
			DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY: // x button on top right corner of window was pressed
		currentApp->SetAppRunningState(false);
		PostQuitMessage(0);
		return 0;
	case WM_INPUT:
		UINT dwSize = sizeof(RAWINPUT);
		static BYTE lpb[sizeof(RAWINPUT)];

		UINT rt = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

		RAWINPUT* raw = (RAWINPUT*)lpb;

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			if (raw->data.mouse.lLastX != 0 || raw->data.mouse.lLastY != 0)
			{
				currentApp->SetMouseDelta(raw->data.mouse.lLastX, raw->data.mouse.lLastY);
			}
		}
		break;
	}
	//if (currentApp)
	//{
		//currentApp->SetMouseDelta(0, 0);
	//}

	//ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);

	return DefWindowProc(hwnd,
		uMsg,
		wParam,
		lParam);
}

Win64App::Win64App()
{
	m_WindowName = "D3D12 Test App";
	m_WindowTitle = "D3D12 App";

	m_Width = 800;
	m_Height = 600;
}

Win64App::~Win64App()
{

}

void Win64App::Init(HINSTANCE hInstance, int windowWidth, int windowHeight)
{
	m_Width = windowWidth;
	m_Height = windowHeight;

	WNDCLASSEX wc = {0};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_WindowName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	auto register_status = RegisterClassEx(&wc);

	m_WindowHandle = CreateWindowEx(NULL,
		m_WindowName,
		m_WindowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_Width, m_Height,
		NULL,
		NULL,
		hInstance,
		NULL);

	// 	DWORD errorMessageID = ::GetLastError();
	// 
	// 	LPSTR messageBuffer = nullptr;
	// 	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	// 		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	// 
	// 	std::string message(messageBuffer, size);


	if (!m_WindowHandle)
	{
		MessageBoxW(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
	}

	ShowWindow(m_WindowHandle, 1);
	UpdateWindow(m_WindowHandle);

	// Make sure the window is on front and focused
	HWND hCurWnd = ::GetForegroundWindow();
	DWORD dwMyID = ::GetCurrentThreadId();
	DWORD dwCurID = ::GetWindowThreadProcessId(hCurWnd, NULL);
	::AttachThreadInput(dwCurID, dwMyID, TRUE);
	::SetWindowPos(m_WindowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	::SetWindowPos(m_WindowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);
	::SetForegroundWindow(m_WindowHandle);
	::SetFocus(m_WindowHandle);
	::SetActiveWindow(m_WindowHandle);
	::AttachThreadInput(dwCurID, dwMyID, FALSE);
	::SetCursor(NULL);

	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = (FLOAT)m_Width;
	m_viewport.Height = (FLOAT)m_Height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	m_scissors.left = 0;
	m_scissors.top = 0;
	m_scissors.right = m_Width;
	m_scissors.bottom = m_Height;

	// register mouse to get raw input
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = RIDEV_INPUTSINK;
	Rid[0].hwndTarget = m_WindowHandle;
	bool h = RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
}

void Win64App::AddLoopCallbacks(std::function<void()> beginCallback, std::function<void()> updateCallback, std::function<void()> endCallback)
{
	m_beginCallback = beginCallback;
	m_updateCallback = updateCallback;
	m_endCallback = endCallback;
}

void Win64App::AppLoop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	Win64App* currentApp2 = DisplayManager::Get()->GetCurrentApp();

	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// Add delta time calculation
			if(m_beginCallback)
				m_beginCallback();

			if (m_updateCallback)
				m_updateCallback();

			if (m_endCallback)
				m_endCallback();
		}
	}
}


