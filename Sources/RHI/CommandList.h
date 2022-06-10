#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <RHI/Descriptor.h>
#include <RHI/ResourceStateTracker.h>

namespace sy::RHI
{
    /**
    * https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12graphicscommandlist
    * https://docs.microsoft.com/en-us/windows/win32/direct3d12/recording-command-lists-and-bundles  => Command list API Restrictions
    */
    class Device;
    class CommandAllocator;
    class DirectCommandAllocator;
    class BundleCommandAllocator;
    class ComputeCommandAllocator;
    class CopyCommandAllocator;
    class Resource;
    class Texture;
    class Buffer;
    class ResourceBarrier;
    class DescriptorHeap;
    class CommandList : public RHIObject
    {
    public:
        virtual ~CommandList() override = 0;

        void SetDebugName(const std::wstring_view debugName) override;

        virtual void Open();
        virtual void Close();

        [[nodiscard]] ID3D12GraphicsCommandList7* GetD3DCommandList() const noexcept { return commandList.Get(); }
        [[nodiscard]] D3D12_COMMAND_LIST_TYPE GetType() const noexcept { return type; }

        [[nodiscard]] class CopyCommandList& ToCopy() noexcept
        {
            assert(type == D3D12_COMMAND_LIST_TYPE_COPY);
            return reinterpret_cast<CopyCommandList&>(*this);
        }

        [[nodiscard]] class ComputeCommandList& ToCompute() noexcept
        {
            assert(type == D3D12_COMMAND_LIST_TYPE_COMPUTE);
            return reinterpret_cast<ComputeCommandList&>(*this);
        }

        [[nodiscard]] class DirectCommandList& ToDirect() noexcept
        {
            assert(type == D3D12_COMMAND_LIST_TYPE_DIRECT);
            return reinterpret_cast<DirectCommandList&>(*this);
        }

        [[nodiscard]] class BundleCommandList& ToBundle() noexcept
        {
            assert(type == D3D12_COMMAND_LIST_TYPE_BUNDLE);
            return reinterpret_cast<BundleCommandList&>(*this);
        }

    protected:
        CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type, const CommandAllocator& cmdAllocator);

    protected:
        const CommandAllocator& cmdAllocator;
        const D3D12_COMMAND_LIST_TYPE type;
        const ComPtr<ID3D12GraphicsCommandList7> commandList;

    };

    template <typename T>
    concept CommandListType = std::derived_from<T, CommandList>;

    class CopyCommandList : public CommandList
    {
    public:
        CopyCommandList(Device& device, const CopyCommandAllocator& commandAllocator);

        void Open() override;
        void Close() override;

        void CopyResource(const Resource& destination, const Resource& source);

        void TransitionBarrier(Resource& target, D3D12_RESOURCE_STATES targetState, uint32 subResourceIndex = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES);

        void UAVBarrier(const Resource* target)
        {
            AppendBarrierToBatch(CD3DX12_RESOURCE_BARRIER::UAV(target != nullptr ? target->GetD3DResource() : nullptr));
        }

        void AliasingBarrier(const Resource* before, const Resource* after)
        {
            AppendBarrierToBatch(CD3DX12_RESOURCE_BARRIER::Aliasing(
                before != nullptr ? before->GetD3DResource() : nullptr,
                after != nullptr ? after->GetD3DResource() : nullptr
            ));
        }

        void FlushResourceBarriers()
        {
            ANYA_ASSERT(numCurrentBatchedBarriers < NumMaxBatchBarriers, "Invalid num of batched barriers");
            if (numCurrentBatchedBarriers > 0)
            {
                GetD3DCommandList()->ResourceBarrier(numCurrentBatchedBarriers, barriersBatch.data());
                numCurrentBatchedBarriers = 0;
            }
        }

        std::vector<D3D12_RESOURCE_BARRIER> ResolvePendingResourceBarriers();

        void AppendBarrierToBatch(D3D12_RESOURCE_BARRIER barrier);

    protected:
        using CommandList::CommandList;

    private:
        static constexpr uint32 NumMaxBatchBarriers = 16;
        ResourceStateTracker resourceStateTracker;
        std::array<D3D12_RESOURCE_BARRIER, NumMaxBatchBarriers> barriersBatch;
        uint32 numCurrentBatchedBarriers = 0;

    };

    /**
    * @brief    Compute = Copy + Compute
    */
    class ComputeCommandList : public CopyCommandList
    {
    public:
        ComputeCommandList(Device& device, const ComputeCommandAllocator& commandAllocator);

        /** Proxy functions for Compute Command List */
        void SetDescriptorHeaps(std::span<ConstRef<DescriptorHeap>> descriptorHeaps) const;

    protected:
        using CopyCommandList::CopyCommandList;

    };

    class DirectCommandListBase : public ComputeCommandList
    {
    public:
        /** Proxy functions for Direct Command List */

        /**
        * @brief Clear Render Target.
        */
        void ClearRenderTarget(RTDescriptor renderTarget, const DirectX::XMFLOAT4& color) const;
        void SetRenderTarget(RTDescriptor renderTarget, std::optional<DSDescriptor> depthStencil = std::nullopt) const;
        void SetRenderTargets(std::span<RTDescriptor> renderTargets, std::optional<DSDescriptor> depthStencil = std::nullopt) const;

    protected:
        using ComputeCommandList::ComputeCommandList;

    };

    /**
    * @brief    Direct = Graphics + Compute + Copy
    */
    class DirectCommandList : public DirectCommandListBase
    {
    public:
        DirectCommandList(Device& device, const DirectCommandAllocator& commandAllocator);

    };

    /**
    * https://docs.microsoft.com/en-us/windows/win32/direct3d12/recording-command-lists-and-bundles
    */
    class BundleCommandList : public DirectCommandListBase
    {
    public:
        BundleCommandList(Device& device, const BundleCommandAllocator& commandAllocator);

    };
}