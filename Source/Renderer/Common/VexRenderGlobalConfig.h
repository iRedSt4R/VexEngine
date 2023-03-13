#pragma once

#include <d3d12.h>
#include <iostream>
#include <windows.h>

namespace RenderGlobalConfig
{
	ID3D12Device* g_device;

	uint32_t g_windowWidth = 1920;
	uint32_t g_windowHeight = 1080;

	uint32_t g_renderWidth = 1920;
	uint32_t g_renderHeight = 1080;

	uint32_t g_dirShadowMapWidth = 3840;
	uint32_t g_dirShadowMapWidth = 2160;

	uint8_t g_renderThreads = 1;
};