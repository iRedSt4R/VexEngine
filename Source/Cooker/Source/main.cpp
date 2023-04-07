#define VEX_COOKER 1
#include <API/../Common/VexRenderCommon.h>

#include <iostream>
#include <filesystem>

#include <API/Meshes/Mesh.h>

int main(int argc, char* argv[])
{
	//std::copy(argv, argv + argc, std::ostream_iterator<char*>(std::cout, "\n"));
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	std::cout << std::filesystem::current_path();

	Mesh* m_mesh = nullptr;
	m_mesh = new Mesh();
	m_mesh->LoadMesh("sponza/Sponza.gltf", "sponza/");

	m_mesh->Serialize("sponaSerialized.vexmesh");

	//std::string firstArg = argv[0];
	//std::string secondArg = argv[1];
	//std::string thirdArg = argv[2];

	//std::cout << std::endl;
	//std::cout << argv[3];

	return 0;
}