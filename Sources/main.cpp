#include <PCH.h>
#include <Core/Application.h>
#include <Core/Exceptions.h>
#include <Core/TaskPool.h>
#include <Components/HierarchyComponent.h>
#include <Components/TagComponents.h>
#include <Components/TransformComponent.h>

sy::int32 main(int argc, wchar_t** argv)
{
	using namespace sy;
	auto app = std::make_unique<Application>(TEXT("AnyaRenderer"), argc, argv);
	std::wcout << app->Title() << std::endl;
	return app->Execute();
}