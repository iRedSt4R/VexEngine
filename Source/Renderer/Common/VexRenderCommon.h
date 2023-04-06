#pragma once

#define NOMINMAX 

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }

#if _DEBUG
#define CHECK_HRESULT(res) { if(res != S_OK) __debugbreak(); }
#else
#define CHECK_HRESULT(res)
#endif

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

#include <iostream>
#include <windows.h>
#include <shellapi.h>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <array>
#include <tuple>
#include <filesystem>
#include <wrl.h>
#include <sstream>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>
#include <fstream>
#include <d3dx12.h>
#include <thread>
#include <chrono>
#include <filesystem>
#include <functional>
#include <filesystem>

using namespace Microsoft::WRL;
using namespace DirectX;

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

#define VEX_RENDER_USE_VALIDATOR 0
#define VEX_RENDER_USE_DEBUG_DEVICE 1

#define VEX_RENDER_INIT_WINDOW_WIDTH 800
#define VEX_RENDER_INIT_WINDOW_HEIGHT 600
#define VEX_RENDER_INIT_SURFACE_WIDTH 800
#define VEX_RENDER_INIT_SURFACE_HEIGHT 600
#define VEX_RENDER_BACK_BUFFER_COUNT 2
#define VEX_TEXTURE_2D_BINDLESS_TABLE_SIZE 4096
#define VEX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE (VEX_TEXTURE_2D_BINDLESS_TABLE_SIZE + 4096)

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Position;
};

//Vertex with position(3 floats) and color(4 floats)
struct VertexP3C4 {
	VertexP3C4() {};
	VertexP3C4(XMFLOAT3 position, XMFLOAT4 color) {
		pos = position;
		col = color;
	}

	XMFLOAT3 pos;
	XMFLOAT4 col;
};

struct VertexP3 {
	XMFLOAT3 pos;
};

//Vertex with position(3 floats) and texcoord(2 floats)
struct VertexP3T2 {
	VertexP3T2() {};
	VertexP3T2(XMFLOAT3 position, XMFLOAT2 coord) {
		pos = position;
		texCoord = coord;
	}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};
