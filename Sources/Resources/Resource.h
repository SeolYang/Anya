#pragma once
#include <PCH.h>

namespace sy
{
    using ResourcePtr = std::shared_ptr<class Resource>;
    using ResourceUniquePtr = std::unique_ptr<class Resource>;

    enum class EResourceType
    {
        Image,
        Model,
        Text,
        Shader,
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

        /** 
        * Load is not thread-safe it self.
        * It must be called by single thread.
        */
        bool Load() 
        { 
            if (!bIsLoaded)
            {
                if (Load_Internal())
                {
                    bIsLoaded.store(true);
                    return true;
                }
            }

            return false;
        }

        /**
        * Unload is not thread-safe it self.
        * It must be called by single thread.
        */
        void Unload() 
        {
            if (bIsLoaded)
            {
                Unload_Internal();
                bIsLoaded = false;
            }
        }

        const fs::path& Path() const noexcept { return path; }
        virtual EResourceType Type() const noexcept = 0;

        bool IsReadyToUse() const noexcept { return bIsLoaded; }

    protected:
        virtual bool Load_Internal() = 0;
        virtual void Unload_Internal() = 0;

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
