#include <PCH.h>
#include <Core/Application.h>
#include <Core/Exceptions.h>
#include <Core/TaskPool.h>
#include <Components/HierarchyComponent.h>
#include <Components/TagComponents.h>

namespace anya
{
	DeclareArchetype(SceneArchetype, HierarchyComponent, Renderable);
	RegisterArchetype(SceneArchetype);

	struct EmptyComponent
	{
	};

	DeclareComponent(EmptyComponent);
	RegisterComponent(EmptyComponent);
}

anya::int32 main(int argc, wchar_t** argv)
{
	using namespace anya;
	SceneArchetype archetypeObject{
		HierarchyComponent{GenerateEntity()},
		Renderable()
	};

	auto& queriedData = QueryArchetypeData<HierarchyComponent>(archetypeObject);
	queriedData.parentEntity = GenerateEntity();

	auto archetypesSubcomponents = AcquireSubComponentsFromArchetype(archetypeObject);

	assert(is_archetype_v<SceneArchetype>);
	assert(!is_archetype_v<int>);
	assert((is_archetype_contains_v<SceneArchetype, HierarchyComponent>));
	assert(!(is_archetype_contains_v<SceneArchetype, EmptyComponent>));

	auto& poolProxy = ComponentPoolProxy::Get();

	Entity e1 = GenerateEntity();
	poolProxy.CreateComponent<SceneArchetype>(e1);

	Entity e2 = GenerateEntity();
	poolProxy.CreateComponent<SceneArchetype>(e2);
	poolProxy.CreateComponent<EmptyComponent>(e2);

	poolProxy.Attach<SceneArchetype>(e1, e2);

	auto filterAllTest = FilterAll<SceneArchetype>({ e1, e2 }, poolProxy);
	filterAllTest = FilterAll<SceneArchetype, EmptyComponent>({ e1, e2 }, poolProxy);
	auto filterAnyTest = FilterAny<SceneArchetype, EmptyComponent>({ e1, e2 }, poolProxy);

	poolProxy.Detach<SceneArchetype>(e2);
	poolProxy.Remove<SceneArchetype>(e2);


	auto app = std::make_unique<anya::Application>(TEXT("AnyaRenderer"), argc, argv);
	std::wcout << app->Title() << std::endl;
	return app->Execute();
}