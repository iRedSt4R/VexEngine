INCLUDE_DIRECTORIES(${RENDER_SRC_DIR})
INCLUDE_DIRECTORIES(${ENGINE_SRC_DIR})
INCLUDE_DIRECTORIES(${TP_D3DX12_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${TP_DXC_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${TP_DIRECTXTEX_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${TP_ASSIMP_INCLUDE_DIR})

LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
LINK_DIRECTORIES(${TP_DIRECTXTEX_LIB_DIR})
LINK_DIRECTORIES(${TP_ASSIMP_LIB_DIR})

file(GLOB_RECURSE SourceFiles
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

add_executable(VexCooker ${SourceFiles})
target_link_libraries(VexCooker ${RENDER_LIBRARY_NAME})
target_link_libraries(VexCooker dxcompiler)
target_link_libraries(VexCooker ${ENGINE_LIBRARY_NAME})
target_compile_features(VexCooker PRIVATE cxx_std_20)

target_compile_definitions(VexCooker PRIVATE
    $<$<CONFIG:DebugCooker>:VEX_COOKER=1>
    $<$<CONFIG:ReleaseCooker>:VEX_COOKER=1>
)

# Need for fopen to use same path inside VS as executable inside /bin dir 
set_property(TARGET VexCooker PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})