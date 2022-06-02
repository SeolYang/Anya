#pragma once
#include <PCH.h>

namespace sy
{
    enum class EResourceType
    {
        Image,
        Model,
        Text,
        Audio,
        Unknown
    };

    class Resource
    {
    public:
        Resource(const fs::path& path) noexcept :
            path(path)
        {
        }

        virtual ~Resource() noexcept(false)
        {
            if (bIsLoaded)
            {
                Unload();
            }
        }

        Resource(const Resource&) = delete;
        Resource(Resource&&) = delete;
        
        Resource& operator=(const Resource&) = delete;
        Resource& operator=(Resource&&) = delete;

        /** 
        * Load is not thread-safe it self.
        * It must be called by single thread.
        */
        bool Load() 
        { 
            if (!bIsLoaded)
            {
                if (LoadInternal())
                {
                    bIsLoaded.store(true);
                    return true;
                }
            }

            return false;
        }

        /**
        * Unload is not thread-safe itself.
        * It must be called by single thread.
        */
        void Unload()
        {
            if (bIsLoaded)
            {
                UnloadInternal();
                bIsLoaded = false;
            }
        }

        const fs::path& Path() const noexcept { return path; }
        virtual EResourceType Type() const noexcept = 0;

        bool IsReadyToUse() const noexcept { return bIsLoaded; }

    protected:
        virtual bool LoadInternal() = 0;
        virtual void UnloadInternal() = 0;

    private:
        std::atomic_bool bIsLoaded;
        fs::path path;

    };

    template <typename T>
    concept ResourceType = requires
    {
        std::is_base_of_v<Resource, T>;
        T::StaticType();
    };
}
