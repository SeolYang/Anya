#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy::RHI
{
    class Resource;
    class ResourceBarrier : public RHIObject
    {
    public:
        using Vector_t = std::vector<std::reference_wrapper<const ResourceBarrier>>;

    public:
        auto D3DResourceBarrier() const noexcept { return resourceBarrier; }
        auto Type() const noexcept { return resourceBarrier.Type; }

    protected:
        ResourceBarrier() = default;

    protected:
        D3D12_RESOURCE_BARRIER resourceBarrier{};

    };

    inline std::vector<D3D12_RESOURCE_BARRIER> ResourceBarriersToD3D(const ResourceBarrier::Vector_t& barriers)
    {
        std::vector<D3D12_RESOURCE_BARRIER> temporary;
        temporary.reserve(barriers.size());
        std::transform(barriers.begin(), barriers.end(), temporary.begin(), [](const ResourceBarrier& barrier)
            {
                return barrier.D3DResourceBarrier();
            });
        return temporary;
    }

    class ResourceTransitionBarrier : public ResourceBarrier
    {
    public:
        ResourceTransitionBarrier(
            const Resource& resource,
            D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter,
            uint32 subResource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

        [[nodiscard]] auto StateBefore() const noexcept { return resourceBarrier.Transition.StateBefore; }
        [[nodiscard]] auto StateAfter() const noexcept { return resourceBarrier.Transition.StateAfter; }
        [[nodiscard]] uint32 SubResource() const noexcept { return resourceBarrier.Transition.Subresource; }
        [[nodiscard]] const auto& TargetResource() const noexcept { return resource; }

    private:
        const Resource& resource;

    };

    class ResourceAliasingBarrier : public ResourceBarrier
    {
    public:
        ResourceAliasingBarrier(const Resource* resourceBefore, const Resource* resourceAfter);

        const auto* ResourceBefore() const noexcept { return resourceBefore; }
        const auto* ResourceAfter() const noexcept { return resourceAfter; }

    private:
        const Resource* resourceBefore;
        const Resource* resourceAfter;

    };

    class ResourceUAVBarrier : public ResourceBarrier
    {
    public:
        ResourceUAVBarrier(const Resource* resource);

        const auto* TargetResource() const noexcept { return resource; }

    private:
        const Resource* resource;

    };
}