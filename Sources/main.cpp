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

	DeclareArchetype(PhysicsArchetype, HierarchyComponent, Renderable, EmptyComponent);
	RegisterArchetype(PhysicsArchetype);
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
	assert(poolProxy.CreateComponent<SceneArchetype>(e1));

	Entity e2 = GenerateEntity();
	assert(poolProxy.CreateComponent<SceneArchetype>(e2));
	assert(poolProxy.CreateComponent<EmptyComponent>(e2));

	Entity e3 = GenerateEntity();
	assert(poolProxy.CreateComponent<PhysicsArchetype>(e3));

	auto queryHierarchyFromSceneArchetype =
		poolProxy.QueryComponent<HierarchyComponent>(e2);
	OptionalRef<const HierarchyComponent> queryHierarchyFromPhysicsArchetype =
		poolProxy.QueryComponent<HierarchyComponent>(e3);

	assert(poolProxy.Attach<SceneArchetype>(e1, e2));
	// This shoud not work, because those are rely on each other archetypes separably.
	assert(!poolProxy.Attach<SceneArchetype>(e1, e3)); 

	auto filterAllTest = FilterAllOf<SceneArchetype>({ e1, e2 }, poolProxy);
	filterAllTest = FilterAllOf<SceneArchetype, EmptyComponent>({ e1, e2 }, poolProxy);
	auto filterAnyTest = FilterAnyOf<SceneArchetype, EmptyComponent>({ e1, e2 }, poolProxy);

	poolProxy.Detach<SceneArchetype>(e2);
	poolProxy.Remove<SceneArchetype>(e2);

	// Meta-Component ID
	constexpr auto id = QueryComponentID<SceneArchetype>();
	constexpr auto subcomponentsID = AcquireSubComponentsFromArchetype(SceneArchetype());

	auto app = std::make_unique<anya::Application>(TEXT("AnyaRenderer"), argc, argv);
	std::wcout << app->Title() << std::endl;
	return app->Execute();
}