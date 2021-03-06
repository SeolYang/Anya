#pragma once
#include <PCH.h>
#include <Resources/Resource.h>

namespace sy
{
    class ResourceCache
    {
    public:
        ResourceCache() = default;
        ~ResourceCache() = default;
        ResourceCache(const ResourceCache&) = delete;
        ResourceCache(ResourceCache&&) = delete;

        ResourceCache& operator=(const ResourceCache&) = delete;
        ResourceCache& operator=(ResourceCache&&) = delete;

        template <ResourceType T>
        [[nodiscard]] bool Register(std::weak_ptr<T> resource)
        {
            if (!resource.expired() && !Contains(resource))
            {
                auto shared = resource->lock();
                cache[shared->Path()] = shared;
                return true;
            }

            return false;
        }

        [[nodiscard]] bool Contains(const fs::path& path) const noexcept 
        {
            return cache.find(path) != cache.cend(); 
        }

        template <ResourceType T>
        std::weak_ptr<T> Query(const fs::path& path)
        {
            if (Contains(path))
            {
                auto weak = cache.at(path);
                if (!weak.expired())
                {
                    return weak;
                }
                else
                {
                    cache.erase(path);
                }
            }

            return std::weak_ptr<T>();
        }

    private:
        std::mutex mutex;
        robin_hood::unordered_map<fs::path, std::weak_ptr<Resource>> cache;

    };
}