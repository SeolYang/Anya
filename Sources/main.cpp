#include <PCH.h>
#include <Core/Application.h>
#include <Core/Exceptions.h>
#include <Core/TaskPool.h>
#include <Components/HierarchyComponent.h>
#include <Components/TagComponents.h>

namespace anya
{
	DeclareArchetype(SceneArchetype, HierarchyComponent, VisibleToRendererTag);
	RegisterArchetype(SceneArchetype);
}

anya::int32 main(int argc, wchar_t** argv)
{
	auto app = std::make_unique<anya::Application>(TEXT("AnyaRenderer"), argc, argv);
	std::wcout << app->Title() << std::endl;

	return app->Execute();
}