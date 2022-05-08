#include <PCH.h>
#include <Core/Application.h>

class Renderable
{
};

class Visible
{
};

int main(int argc, wchar_t** argv)
{
	using namespace anya;

	int test = 0;
	auto entity = ecs::GenerateEntity();
	ecs::ComponentPool<Renderable> renderableTagPool;
	ecs::ComponentPool<Visible> visibleTagPool;
	renderableTagPool.Create(entity);
	visibleTagPool.Create(entity);

	assert(ecs::IsContains(entity, renderableTagPool, visibleTagPool));
	assert(ecs::IsContains(entity, renderableTagPool));
	assert(ecs::IsContains(entity, visibleTagPool));

	// # Invalid Template instantiation cases
	// * Invalid Type for IsContains
	//assert(ecs::IsContains(entity, renderableTagPool, visibleTagPool, test));
	// * IsContains method must needs more than one pool arguments.
	//assert(ecs::IsContains(entity));

	auto emptyEntity = ecs::GenerateEntity();
	assert(!ecs::IsContains(emptyEntity, renderableTagPool, visibleTagPool));
	assert(!ecs::IsContains(emptyEntity, visibleTagPool));
	assert(!ecs::IsContains(emptyEntity, renderableTagPool));

	auto aComponentEntity = ecs::GenerateEntity();
	auto bComponentEntity = ecs::GenerateEntity();
	renderableTagPool.Create(aComponentEntity);
	visibleTagPool.Create(bComponentEntity);

	assert(ecs::IsContains(aComponentEntity, renderableTagPool));
	assert(!ecs::IsContains(aComponentEntity, visibleTagPool));
	assert(!ecs::IsContains(aComponentEntity, renderableTagPool, visibleTagPool));

	assert(ecs::IsContains(bComponentEntity, visibleTagPool));
	assert(!ecs::IsContains(bComponentEntity, renderableTagPool));
	assert(!ecs::IsContains(bComponentEntity, renderableTagPool, visibleTagPool));

	std::vector<ecs::Entity> group{entity, emptyEntity, aComponentEntity, bComponentEntity};
	auto groupA = ecs::Filter(group, renderableTagPool);
	auto groupB = ecs::Filter(group, visibleTagPool);
	auto groupC = ecs::Filter(group, renderableTagPool, visibleTagPool);

	return 0;
}