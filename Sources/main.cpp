#include <taskflow/taskflow.hpp>
#include <imgui/imgui.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <D3D12MemoryAllocator/D3D12MemAlloc.h>
#include <assimp/Importer.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <Core/ECS.h>

int main(int argc, char** argv)
{
	int someValue = 0;
	std::cout << sy::ecs::ComponentID::Value(someValue) << std::endl;
	std::cout << sy::ecs::ComponentID::Value<int>() << std::endl;
	std::cout << sy::ecs::ComponentID::Value<int>() << std::endl;

	std::cout << sy::ecs::ComponentID::Value<float>() << std::endl;
	std::cout << sy::ecs::ComponentID::Value<const float&>() << std::endl;

	return 0;
}