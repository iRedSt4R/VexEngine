cmake_minimum_required(VERSION 3.13)

# CMake Settings
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PROJECT_NAME "VexEngine")
set(BUILD_FOLDER_NAME "Build")

# Project
project (${PROJECT_NAME})

add_compile_options(/MP)

#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# Main Dirs
set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(THIRD_PARTY_LIB_DIR "${ROOT_DIR}/ThirdParty")
set(SOURCE_DIR "${ROOT_DIR}/Source")

# VexRender
set(RENDER_SRC_DIR "${ROOT_DIR}/Source/Renderer")
set(RENDER_OUTPUT_DIR "${ROOT_SLN_DIR}/Build/Libs")
set(RENDER_LIBRARY_NAME "VexRenderer")

# VexEditor
set(EDITOR_SRC_DIR "${ROOT_DIR}/Source/Editor")
set(EDITOR_OUTPUT_DIR "${ROOT_SLN_DIR}/Build/Libs")
set(EDITOR_LIBRARY_NAME "VexEditor")

# VexEngine
set(ENGINE_SRC_DIR "${ROOT_DIR}/Source/Engine")
set(ENGINE_OUTPUT_DIR "${ROOT_SLN_DIR}/Build/Libs")
set(ENGINE_LIBRARY_NAME "VexEngine")

# VexSandbox
set(SANDBOX_SRC_DIR "${ROOT_SLN_DIR}/Source/Sandbox")
set(SANDBOX_OUTPUT_DIR "${ROOT_SLN_DIR}/Build/Bin64")
set(SANDBOX_EXE_NAME "VexSandbox")

# VexCooker
set(SANDBOX_SRC_DIR "${ROOT_SLN_DIR}/Source/Cooker")
set(SANDBOX_OUTPUT_DIR "${ROOT_SLN_DIR}/Build/Bin64")
set(SANDBOX_EXE_NAME "VexCooker")

# Third-Party directories
# D3DX12
set(TP_D3DX12_INCLUDE_DIR "${THIRD_PARTY_LIB_DIR}/D3DX12/Include")

# Assimp
set(TP_ASSIMP_INCLUDE_DIR "${THIRD_PARTY_LIB_DIR}/Assimp/Include")
set(TP_ASSIMP_LIB_DIR "${THIRD_PARTY_LIB_DIR}/Assimp/Lib")

# DirectXTex
set(TP_DIRECTXTEX_INCLUDE_DIR "${THIRD_PARTY_LIB_DIR}/DirectXTex/Include")
set(TP_DIRECTXTEX_LIB_DIR "${THIRD_PARTY_LIB_DIR}/DirectXTex/Lib")

# DirectXTex
set(TP_DXC_INCLUDE_DIR "${THIRD_PARTY_LIB_DIR}/Dxc/inc")
set(TP_DXC_LIB_DIR "${THIRD_PARTY_LIB_DIR}/Dxc/lib")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Bin")

# Sources
add_subdirectory(Source)