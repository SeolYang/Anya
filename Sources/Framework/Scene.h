#pragma once
#include <PCH.h>
#include <Core/ECS.h>

namespace sy
{
    class Model;
    enum class ESkyType;
    class Scene
    {
    public:
        Scene(ComponentArchive& componentArchive) noexcept;
        virtual ~Scene() noexcept(false);

        Entity CreateSceneEntity(const Entity parent = INVALID_ENTITY_HANDLE);
        Entity CreateStaticMesh(std::weak_ptr<Model> model);
        Entity CreateSkeletalMesh(std::weak_ptr<Model> model);
        Entity CreateLight();
        Entity CreateCamera();
        Entity CreateSky(ESkyType);

        void Init();

        [[nodiscard]] const std::vector<Entity>& GetEntities() const noexcept { return entities; }

    protected:
        [[nodiscard]] virtual bool InitInternal() { return true; };

    private:
        ComponentArchive& componentArchive;
        std::vector<Entity> entities;

    };
}