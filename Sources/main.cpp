#include <PCH.h>
#include <Core/Application.h>

class Renderable
{
};

class Visible
{
};

int main(int argc, char** argv)
{
	int test = 0;
	auto entity = sy::ecs::GenerateEntity();
	sy::ecs::ComponentPool<Renderable> renderableTagPool;
	sy::ecs::ComponentPool<Visible> visibleTagPool;
	renderableTagPool.Create(entity);
	visibleTagPool.Create(entity);

	assert(sy::ecs::IsContains(entity, renderableTagPool, visibleTagPool));
	assert(sy::ecs::IsContains(entity, renderableTagPool));
	assert(sy::ecs::IsContains(entity, visibleTagPool));

	// # Invalid Template instantiation cases
	// * Invalid Type for IsContains
	//assert(sy::ecs::IsContains(entity, renderableTagPool, visibleTagPool, test));
	// * IsContains method must needs more than one pool arguments.
	//assert(sy::ecs::IsContains(entity));

	auto emptyEntity = sy::ecs::GenerateEntity();
	assert(!sy::ecs::IsContains(emptyEntity, renderableTagPool, visibleTagPool));
	assert(!sy::ecs::IsContains(emptyEntity, visibleTagPool));
	assert(!sy::ecs::IsContains(emptyEntity, renderableTagPool));

	auto aComponentEntity = sy::ecs::GenerateEntity();
	auto bComponentEntity = sy::ecs::GenerateEntity();
	renderableTagPool.Create(aComponentEntity);
	visibleTagPool.Create(bComponentEntity);

	assert(sy::ecs::IsContains(aComponentEntity, renderableTagPool));
	assert(!sy::ecs::IsContains(aComponentEntity, visibleTagPool));
	assert(!sy::ecs::IsContains(aComponentEntity, renderableTagPool, visibleTagPool));

	assert(sy::ecs::IsContains(bComponentEntity, visibleTagPool));
	assert(!sy::ecs::IsContains(bComponentEntity, renderableTagPool));
	assert(!sy::ecs::IsContains(bComponentEntity, renderableTagPool, visibleTagPool));

	std::vector<sy::ecs::Entity> group{entity, emptyEntity, aComponentEntity, bComponentEntity};
	auto groupA = sy::ecs::Filter(group, renderableTagPool);
	auto groupB = sy::ecs::Filter(group, visibleTagPool);
	auto groupC = sy::ecs::Filter(group, renderableTagPool, visibleTagPool);

	return 0;
}