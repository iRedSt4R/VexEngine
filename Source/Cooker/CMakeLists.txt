INCLUDE_DIRECTORIES(${RENDER_SRC_DIR})
INCLUDE_DIRECTORIES(${TP_D3DX12_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${TP_DXC_INCLUDE_DIR})

LINK_DIRECTORIES(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

file(GLOB_RECURSE SourceFiles
${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

add_executable(VexCooker WIN32 ${SourceFiles})
target_link_libraries(VexCooker ${RENDER_LIBRARY_NAME})
target_link_libraries(VexSandbox dxcompiler)

# Need for fopen to use same path inside VS as executable inside /bin dir 
set_property(TARGET VexSandbox PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration)")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})