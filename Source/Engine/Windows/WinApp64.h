#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif
#include <windows.h>
#include <shellapi.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <d3d12.h>
#include <hidusage.h>
#include <cmath>
#include <functional>

class Win64App
{
public:
	Win64App();
	~Win64App();
	void Init(HINSTANCE hInstance, int windowWidth, int windowHeight);

	__forceinline HWND GetWindowHandle() { return m_WindowHandle; }
	__forceinline RECT GetWindowRect() { return m_WindowRect; }

	__forceinline D3D12_VIEWPORT GetD3D12Viewport() { return m_viewport; }
	__forceinline D3D12_RECT GetD3D12Scissors() { return m_scissors; }

	__forceinline void SetAppRunningState(bool runningState) { m_Running = runningState; }

	__forceinline void SetMouseDelta(int x, int y) { m_MouseDeltaX = x; m_MouseDeltaY = y; }
	__forceinline int GetMouseDeltaX() { return m_MouseDeltaX; }
	__forceinline int GetMouseDeltaY() { return m_MouseDeltaY; }

	__forceinline int GetWindowHeight() { return m_Height; }
	__forceinline int GetWindowWidth() { return m_Width; }

	void AddLoopCallbacks(std::function<void()> beginCallback, std::function<void()> updateCallback, std::function<void()> endCallback);
	void AppLoop();
protected:

private:
	HWND m_WindowHandle;
	RECT m_WindowRect;

	LPCTSTR m_WindowName;
	LPCTSTR m_WindowTitle;

	int m_Width, m_Height;
	bool m_IsFullScreen;
	bool m_Running;

	std::function<void()> m_beginCallback = nullptr;
	std::function<void()> m_updateCallback = nullptr;
	std::function<void()> m_endCallback = nullptr;

	//Time
	std::chrono::steady_clock::time_point m_t1;
	std::chrono::steady_clock::time_point m_t2;

	//D3D12 stuff
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissors;

	// mouse input 
	RAWINPUTDEVICE Rid[1];
	int m_MouseDeltaX = 0;
	int m_MouseDeltaY = 0;

};