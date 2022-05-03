#include <PCH.h>
#include <Core/ECS.h>
#include <Components/HierarchyComponent.h>

namespace PrintHierarchySystem
{
	void PrintSubtree(const sy::ecs::ComponentPool<sy::ecs::HierarchyComponent>& pool, size_t rootIdx = 0, int depth = 0)
	{
		auto entityOpt = pool.GetEntity(rootIdx);
		if (entityOpt.has_value())
		{
			const auto& rootEntity = entityOpt.value();
			for (int itr = 0; itr < depth; ++itr)
			{
				std::cout << '-';
			}
			std::cout << " ID : " << rootEntity << std::endl;

			for (size_t childIdx = rootIdx + 1; childIdx < pool.Size(); ++childIdx)
			{
				if (pool[childIdx].parentEntity == rootEntity)
				{
					PrintSubtree(pool, childIdx, depth + 1);
				}
			}
		}
	}

	void PrintHierarchy(const sy::ecs::ComponentPool<sy::ecs::HierarchyComponent>& pool)
	{
		std::cout << "ROOT" << std::endl;
		for (size_t idx = 0; idx < pool.Size(); ++idx)
		{
			if (pool[idx].parentEntity == sy::ecs::INVALID_ENTITY_HANDLE)
			{
				PrintSubtree(pool, idx, 1);
			}
		}

		std::cout << std::endl;
	}
}

int main(int argc, char** argv)
{
	sy::ecs::ComponentPoolRegistry* registry = sy::ecs::ComponentPoolRegistry::GetGlobalInitRegistry(true);
	auto pool = registry->AcquireWithComponentType<sy::ecs::HierarchyComponent>();
	assert(pool != nullptr);

	sy::ecs::ComponentPool<sy::ecs::HierarchyComponent>& hierarchyPool = *pool;

	std::vector<sy::ecs::Entity> entities;
	sy::ecs::Entity root = sy::ecs::GenerateEntity();
	hierarchyPool.Create(root);

	for (int idx = 0; idx < 6; ++idx)
	{
		entities.push_back(sy::ecs::GenerateEntity());
		hierarchyPool.Create(entities[idx]);
	}

	std::cout << "- Hierarchy -" << std::endl;
	PrintHierarchySystem::PrintHierarchy(hierarchyPool);
	hierarchyPool.Attach(entities[4], entities[3]);
	PrintHierarchySystem::PrintHierarchy(hierarchyPool);
	hierarchyPool.Attach(entities[0], entities[2]);
	PrintHierarchySystem::PrintHierarchy(hierarchyPool);
	hierarchyPool.Attach(root, entities[0]);
	PrintHierarchySystem::PrintHierarchy(hierarchyPool);
	hierarchyPool.Attach(root, entities[4]);
	PrintHierarchySystem::PrintHierarchy(hierarchyPool);
	hierarchyPool.Attach(entities[0], entities[1]);
	PrintHierarchySystem::PrintHierarchy(hierarchyPool);
	hierarchyPool.Attach(entities[3], entities[5]);
	PrintHierarchySystem::PrintHierarchy(hierarchyPool);
	hierarchyPool.Detach(entities[4]);
	hierarchyPool.Remove(entities[4]);
	PrintHierarchySystem::PrintHierarchy(hierarchyPool);

	return 0;
}