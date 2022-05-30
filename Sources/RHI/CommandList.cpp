#include <PCH.h>
#include <RHI/CommandList.h>
#include <RHI/Device.h>
#include <RHI/CommandAllocator.h>
#include <RHI/Resource.h>
#include <RHI/Texture.h>
#include <RHI/ResourceBarrier.h>
#include <RHI/Descriptor.h>
#include <Core/Exceptions.h>

namespace sy::RHI
{
    CommandList::CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type, const CommandAllocator& cmdAllocator) :
        cmdAllocator(cmdAllocator)
    {
        DXCall(device.D3DDevice()->CreateCommandList(device.NodeMask(), type, cmdAllocator.D3DCommandAllocator(), nullptr, IID_PPV_ARGS(&commandList)));
        /** Command List is on recording state. So close it at here to make able to call reset later. */
        Close();
        SetDebugName(TEXT("CommandList"));
    }

    void CommandList::SetDebugName(const std::wstring_view debugName)
    {
        RHIObject::SetDebugName(debugName);
        if (commandList != nullptr)
        {
            DXCall(commandList->SetName(debugName.data()));
        }
    }

    void CommandList::Reset()
    {
        if (commandList != nullptr)
        {
            DXCall(commandList->Reset(cmdAllocator.D3DCommandAllocator(), nullptr));
        }
    }

    void CommandList::Close()
    {
        if (commandList != nullptr)
        {
            DXCall(commandList->Close());
        }
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

    ComputeCommandList::ComputeCommandList(Device& device, const ComputeCommandAllocator& commandAllocator) :
        ComputeCommandListBase(device, D3D12_COMMAND_LIST_TYPE_COMPUTE, commandAllocator)
    {
        SetDebugName(TEXT("ComputeCommandList"));
    }

    CopyCommandList::CopyCommandList(Device& device, const CopyCommandAllocator& commandAllocator) :
        CopyCommandListBase(device, D3D12_COMMAND_LIST_TYPE_COPY, commandAllocator)
    {
        SetDebugName(TEXT("CopyCommandList"));
    }

    void CopyCommandListBase::CopyResource(const Resource& destination, const Resource& source)
    {
        assert((destination.D3DResource() != nullptr) && (source.D3DResource() != nullptr));
        D3DCommandList()->CopyResource(destination.D3DResource(), source.D3DResource());
    }

    void CopyCommandListBase::AppendResourceBarrier(const ResourceBarrier& barrier)
    {
        const auto targetBarrier = barrier.D3DResourceBarrier();
        D3DCommandList()->ResourceBarrier(1, &targetBarrier);
    }

    void CopyCommandListBase::AppendResourceBarriers(const ResourceBarrier::Vector_t& barriers)
    {
        const auto targetBarriers = ResourceBarriersToD3D(barriers);
        D3DCommandList()->ResourceBarrier((uint32)barriers.size(), targetBarriers.data());
    }

    void DirectCommandListBase::ClearRenderTarget(const RTDescriptor& rtDescriptor, const DirectX::XMFLOAT4& color)
    {
        D3DCommandList()->ClearRenderTargetView(rtDescriptor.CPUHandle(), &color.x, 0, nullptr);
    }
}