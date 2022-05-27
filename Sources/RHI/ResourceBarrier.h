#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy
{
    class RHIResource;
    class ResourceBarrier : public RHIObject
    {
    public:
        ResourceBarrier() = default;

        auto D3DResourceBarrier() const noexcept { return resourceBarrier; }
        auto Type() const noexcept { return resourceBarrier.Type; }

    protected:
        D3D12_RESOURCE_BARRIER resourceBarrier{};

    };

    inline std::vector<D3D12_RESOURCE_BARRIER> ResourceBarriersToD3D(const std::vector<ResourceBarrier>& barriers)
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
            const RHIResource& resource,
            D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter,
            uint32 subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

        auto StateBefore() const noexcept { return resourceBarrier.Transition.StateBefore; }
        auto StateAfter() const noexcept { return resourceBarrier.Transition.StateAfter; }
        auto Subresource() const noexcept { return resourceBarrier.Transition.Subresource; }
        const auto& TargetResource() const noexcept { return resource; }

    private:
        const RHIResource& resource;

    };

    class ResourceAliasingBarrier : public ResourceBarrier
    {
    public:
        ResourceAliasingBarrier(const RHIResource& resourceBefore, const RHIResource& resourceAfter);

        const auto& ResourceBefore() const noexcept { return resourceBefore; }
        const auto& ResourceAfter() const noexcept { return resourceAfter; }

    private:
        const RHIResource& resourceBefore;
        const RHIResource& resourceAfter;

    };

    class ResourceUAVBarrier : public ResourceBarrier
    {
    public:
        ResourceUAVBarrier(const RHIResource& resource);

        const auto& TargetResource() const noexcept { return resource; }

    private:
        const RHIResource& resource;

    };
}