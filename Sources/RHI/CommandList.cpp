#include <PCH.h>
#include <RHI/CommandList.h>
#include <RHI/Device.h>
#include <RHI/CommandAllocator.h>
#include <RHI/RHIResource.h>
#include <RHI/Texture.h>
#include <Core/Exceptions.h>

namespace sy
{
    CommandList::CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type, CommandAllocator& commandAllocator) :
        commandAllocator(commandAllocator)
    {
        ComPtr<ID3D12GraphicsCommandList> _commandList;
        DXCall(device.D3DDevice()->CreateCommandList(device.NodeMask(), type, commandAllocator.D3DCommandAllocator(), nullptr, IID_PPV_ARGS(&_commandList)));
        DXCall(_commandList.As(&commandList));
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
            DXCall(commandList->Reset(commandAllocator.D3DCommandAllocator(), nullptr));
        }
    }

    void CommandList::Close()
    {
        if (commandList != nullptr)
        {
            DXCall(commandList->Close());
        }
    }

    DirectCommandList::DirectCommandList(Device& device, DirectCommandAllocator& commandAllocator) :
        DirectCommandListBase(device, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator)
    {
        SetDebugName(TEXT("DirectCommandList"));
    }

    BundleCommandList::BundleCommandList(Device& device, BundleCommandAllocator& commandAllocator) :
        DirectCommandListBase(device, D3D12_COMMAND_LIST_TYPE_BUNDLE, commandAllocator)
    {
        SetDebugName(TEXT("BundleCommandList"));
    }

    ComputeCommandList::ComputeCommandList(Device& device, ComputeCommandAllocator& commandAllocator) :
        ComputeCommandListBase(device, D3D12_COMMAND_LIST_TYPE_COMPUTE, commandAllocator)
    {
        SetDebugName(TEXT("ComputeCommandList"));
    }

    CopyCommandList::CopyCommandList(Device& device, CopyCommandAllocator& commandAllocator) :
        CopyCommandListBase(device, D3D12_COMMAND_LIST_TYPE_COPY, commandAllocator)
    {
        SetDebugName(TEXT("CopyCommandList"));
    }

    void CopyCommandListBase::CopyResource(const RHIResource& destination, const RHIResource& source)
    {
        assert((destination.D3DResource() != nullptr) && (source.D3DResource() != nullptr));
        D3DCommandList()->CopyResource(destination.D3DResource(), source.D3DResource());
    }

    void CopyCommandListBase::AppendResourceBarrier(const ResourceBarrier& barrier)
    {
    }

    void CopyCommandListBase::AppendResourceBarriers(const std::vector<ResourceBarrier>& barriers)
    {
    }
}