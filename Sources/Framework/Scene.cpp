#include <PCH.h>
#include <Framework/Scene.h>
#include <Components/HierarchyComponent.h>
#include <Components/TagComponents.h>
#include <Components/TransformComponent.h>

namespace sy
{
    Scene::Scene(ComponentArchive& componentArchive) noexcept :
        componentArchive(componentArchive)
    {
    }

    Scene::~Scene() noexcept(false)
    {
        for (const Entity entity : entities)
        {
            if (entity != INVALID_ENTITY_HANDLE)
            {
                componentArchive.Destroy(entity);
            }
        }

        if (entities.size() > 0)
        {
            componentArchive.ShrinkToFit();
        }
    }

    Entity Scene::CreateSceneEntity(const Entity parent)
    {
        assert(parent == INVALID_ENTITY_HANDLE || 
            (componentArchive.Contains<HierarchyComponent>(parent) && componentArchive.Contains<TransformComponent>(parent)));

        Entity newEntity = GenerateEntity();
        componentArchive.Attach<HierarchyComponent>(newEntity, parent);
        componentArchive.Attach<TransformComponent>(newEntity);

        return newEntity;
    }
}