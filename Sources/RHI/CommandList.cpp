#include <PCH.h>
#include <RHI/CommandList.h>
#include <RHI/Device.h>
#include <RHI/CommandAllocator.h>
#include <RHI/Resource.h>
#include <RHI/Descriptor.h>
#include <RHI/DescriptorHeap.h>
#include <Core/Exceptions.h>
#include <Core/Assert.h>


namespace sy::RHI
{
    CommandList::CommandList(Device& device, const D3D12_COMMAND_LIST_TYPE type, const CommandAllocator& cmdAllocator) :
        cmdAllocator(cmdAllocator),
        type(type),
        commandList([&]
        {
            ComPtr<ID3D12GraphicsCommandList7> cmdList;
            DXCall(device.GetD3DDevice()->CreateCommandList(device.GetNodeMask(), type, cmdAllocator.GetD3DCommandAllocator(), nullptr, IID_PPV_ARGS(&cmdList)));
            return cmdList;
        }())
    {
        
        /** Command List is on recording state. So close it at here to make able to call reset later. */
        this->Close();
    }

    CommandList::~CommandList()
    {
    }

    void CommandList::Open()
    {
        ANYA_ASSERT(commandList != nullptr, "D3D Command List was null.");
        DXCall(commandList->Reset(cmdAllocator.GetD3DCommandAllocator(), nullptr));
    }

    void CommandList::Close()
    {
        ANYA_ASSERT(commandList != nullptr, "D3D Command List was null.");
        DXCall(commandList->Close());
    }

    void CommandList::SetDebugName(const std::wstring_view debugName)
    {
        RHIObject::SetDebugName(debugName);
        if (commandList != nullptr)
        {
            DXCall(commandList->SetName(debugName.data()));
        }
    }

    CopyCommandList::CopyCommandList(Device& device, const CopyCommandAllocator& commandAllocator) :
        CommandList(device, D3D12_COMMAND_LIST_TYPE_COPY, commandAllocator)
    {
        SetDebugName(TEXT("CopyCommandList"));
    }

    void CopyCommandList::Open()
    {
        CommandList::Open();
        resourceStateTracker.Reset();
        numCurrentBatchedBarriers = 0;
    }

    void CopyCommandList::Close()
    {
        FlushResourceBarriers();
        CommandList::Close();
    }

    void CopyCommandList::CopyResource(const Resource& destination, const Resource& source)
    {
        assert((destination.GetD3DResource() != nullptr) && (source.GetD3DResource() != nullptr));
        GetD3DCommandList()->CopyResource(destination.GetD3DResource(), source.GetD3DResource());
    }

    void CopyCommandList::TransitionBarrier(Resource& target, const D3D12_RESOURCE_STATES targetState, const uint32 subResourceIndex)
    {
        ANYA_ASSERT(target.GetD3DResource() != nullptr, "Resource can't be a null resource to state transition.")
        ANYA_ASSERT(targetState != D3D12_RESOURCE_STATE_UNKNOWN && targetState != D3D12_RESOURCE_STATE_BEFORE_INITIALIZE, "Not a valid resource state to transition.");

        ResourceState& resState = resourceStateTracker.GetResourceState(&target);
        ANYA_ASSERT(resState.IsInitialized(), "Not initialized Resource State.");

        // If resource state is unknown. It must be resolved later!
        if (resState.IsUnknownState())
        {
            resourceStateTracker.AddPendingResourceState(
                ResourceStateTracker::PendingResourceState{
                    .Target = &target,
                    .State = targetState,
                    .SubResourceIndex = subResourceIndex
                }
            );
        }
        else
        {
            if (subResourceIndex == D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES && !resState.IsTrackingPerResource())
            {
                uint32 idx = 0;
                for (const D3D12_RESOURCE_STATES subResourceState : resState.GetSubResourceStates())
                {
                    if (subResourceState != targetState)
                    {
                        AppendBarrierToBatch(CD3DX12_RESOURCE_BARRIER::Transition(
                            target.GetD3DResource(),
                            subResourceState,
                            targetState,
                            idx));
                    }

                    ++idx;
                }
            }
            else
            {
                const auto beforeSubResourceState = resState.GetSubResourceState(subResourceIndex);
                if (beforeSubResourceState != targetState)
                {
                    AppendBarrierToBatch(CD3DX12_RESOURCE_BARRIER::Transition(
                        target.GetD3DResource(),
                        beforeSubResourceState,
                        targetState,
                        subResourceIndex));
                }
            }
        }

        resState.SetSubResourceState(subResourceIndex, targetState);
    }

    std::vector<D3D12_RESOURCE_BARRIER> CopyCommandList::ResolvePendingResourceBarriers()
    {
        std::vector<D3D12_RESOURCE_BARRIER> resolvedBarriers;
        for (ResourceStateTracker::PendingResourceState& pendingResourceState : resourceStateTracker.GetPendingResourceStates())
        {
            // Resolve before state from Resource.
            auto& resState = pendingResourceState.Target->GetResourceState();
            const auto currentSubResourceState = resState.GetSubResourceState(pendingResourceState.SubResourceIndex);
            const auto targetState = pendingResourceState.State;
            if (currentSubResourceState != targetState)
            {
                resolvedBarriers.emplace_back(
                    CD3DX12_RESOURCE_BARRIER::Transition(
                        pendingResourceState.Target->GetD3DResource(),
                        currentSubResourceState,
                        targetState,
                        pendingResourceState.SubResourceIndex));
            }

            // Update Resource state using ResourceStateTracker's infos.
            const auto finalResStateInCmdList = resourceStateTracker.GetResourceState(pendingResourceState.Target).GetSubResourceState(pendingResourceState.SubResourceIndex);
            resState.SetSubResourceState(pendingResourceState.SubResourceIndex, finalResStateInCmdList);
        }

        return resolvedBarriers;
    }

    void CopyCommandList::AppendBarrierToBatch(const D3D12_RESOURCE_BARRIER barrier)
    {
        ANYA_ASSERT(numCurrentBatchedBarriers < NumMaxBatchBarriers, "Can't exceed maximum batch barrier capacity.");
        barriersBatch[numCurrentBatchedBarriers] = barrier;
        ++numCurrentBatchedBarriers;
        if (numCurrentBatchedBarriers == NumMaxBatchBarriers)
        {
            FlushResourceBarriers();
        }
    }

    ComputeCommandList::ComputeCommandList(Device& device, const ComputeCommandAllocator& commandAllocator) :
        CopyCommandList(device, D3D12_COMMAND_LIST_TYPE_COMPUTE, commandAllocator)
    {
        SetDebugName(TEXT("ComputeCommandList"));
    }

    void ComputeCommandList::SetDescriptorHeaps(std::span<ConstRef<DescriptorHeap>> descriptorHeaps) const
    {
        if (descriptorHeaps.size() > 0)
        {
            std::vector<ID3D12DescriptorHeap*> d3dDescriptorHeaps{descriptorHeaps.size()};
            ranges::transform(descriptorHeaps.begin(), descriptorHeaps.end(), d3dDescriptorHeaps.begin(),
                [](const ConstRef<DescriptorHeap>& descriptorHeapRef)
                {
                    return descriptorHeapRef.get().GetD3DDescriptorHeap();
                });

            GetD3DCommandList()->SetDescriptorHeaps(static_cast<uint32>(d3dDescriptorHeaps.size()), d3dDescriptorHeaps.data());
        }
    }

    void DirectCommandListBase::ClearRenderTarget(const RTDescriptor renderTarget, const DirectX::XMFLOAT4& color) const
    {
        GetD3DCommandList()->ClearRenderTargetView(renderTarget.GetCPUHandle(), &color.x, 0, nullptr);
    }

    void DirectCommandListBase::SetRenderTarget(const RTDescriptor renderTarget, const std::optional<DSDescriptor> depthStencil) const
    {
        GetD3DCommandList()->OMSetRenderTargets(
            1, 
            &renderTarget.GetCPUHandle(), 
            false,
            depthStencil.has_value() ? &depthStencil->GetCPUHandle() : nullptr);
    }

    void DirectCommandListBase::SetRenderTargets(std::span<RTDescriptor> renderTargets, std::optional<DSDescriptor> depthStencil) const
    {
        /**
         * @TODO Implement Set Multiple Render Targets
         */
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> d3dDescriptorHandles{ renderTargets.size()};
        ranges::transform(renderTargets.begin(), renderTargets.end(), d3dDescriptorHandles.begin(),
            [](const RTDescriptor& rtDescriptor)
            {
                return rtDescriptor.GetCPUHandle();
            });

        GetD3DCommandList()->OMSetRenderTargets(
            static_cast<uint32>(d3dDescriptorHandles.size()),
            d3dDescriptorHandles.size() > 0 ? d3dDescriptorHandles.data() : nullptr,
            false,
            depthStencil.has_value() ? &depthStencil->GetCPUHandle() : nullptr);
    }

    DirectCommandList::DirectCommandList(Device& device, const DirectCommandAllocator& commandAllocator) :
        DirectCommandListBase(device, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator)
    {
        SetDebugName(TEXT("DirectCommandList"));
    }

    BundleCommandList::BundleCommandList(Device& device, const BundleCommandAllocator& commandAllocator) :
        DirectCommandListBase(device, D3D12_COMMAND_LIST_TYPE_BUNDLE, commandAllocator)
    {
        SetDebugName(TEXT("BundleCommandList"));
    }
 
}
