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
        Scene(Logger& logger, ComponentArchive& componentArchive) noexcept;
        virtual ~Scene() noexcept(false);

        Entity CreateSceneEntity(const Entity parent = INVALID_ENTITY_HANDLE);
        Entity CreateStaticMesh(std::weak_ptr<Model> model);
        Entity CreateSkeletalMesh(std::weak_ptr<Model> model);
        Entity CreateLight();
        Entity CreateCamera();
        Entity CreateSky(ESkyType);

        void Init()
        { 
            logger.info("Initializing Scene...");
            if (Init_Internal())
            {
                logger.info("Scene Initialized.");
            }
            else
            {
                logger.error("Failed to initialize Scene.");
            }
        }

        const std::vector<Entity>& Entities() const noexcept { return entities; }

    protected:
        virtual bool Init_Internal() { return true; };

    private:
        Logger& logger;
        ComponentArchive& componentArchive;
        std::vector<Entity> entities;

    };
}