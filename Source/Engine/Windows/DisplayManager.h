#pragma once

#include "WinApp64.h"

class DisplayManager
{
public:
	static DisplayManager* Get()
	{
		if (s_display_manager == nullptr)
			s_display_manager = new DisplayManager();

		return s_display_manager;
	}

	__forceinline void SetCurrentApp(Win64App* app) { m_CurrentApp = app; }
	__forceinline Win64App* GetCurrentApp() { return m_CurrentApp; }

private:
	DisplayManager() {};
	~DisplayManager() {};

public:
private:
	static DisplayManager* s_display_manager;

	Win64App* m_CurrentApp = nullptr;
};