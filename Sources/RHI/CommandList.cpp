#include <PCH.h>
#include <RHI/CommandList.h>
#include <RHI/Device.h>
#include <RHI/CommandAllocator.h>
#include <Core/Exceptions.h>

namespace sy
{
    CommandList::CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type, CommandAllocator& commandAllocator) :
        commandAllocator(commandAllocator)
    {
        DXCall(device.D3DDevice()->CreateCommandList(device.NodeMask(), type, commandAllocator.D3DCommandAllocator(), nullptr, IID_PPV_ARGS(&commandList)));
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
        CommandList(device, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator)
    {
        SetDebugName(TEXT("DirectCommandList"));
    }

    BundleCommandList::BundleCommandList(Device& device, BundleCommandAllocator& commandAllocator) :
        CommandList(device, D3D12_COMMAND_LIST_TYPE_BUNDLE, commandAllocator)
    {
        SetDebugName(TEXT("BundleCommandList"));
    }

    ComputeCommandList::ComputeCommandList(Device& device, ComputeCommandAllocator& commandAllocator) :
        CommandList(device, D3D12_COMMAND_LIST_TYPE_COMPUTE, commandAllocator)
    {
        SetDebugName(TEXT("ComputeCommandList"));
    }

    CopyCommandList::CopyCommandList(Device& device, CopyCommandAllocator& commandAllocator) :
        CommandList(device, D3D12_COMMAND_LIST_TYPE_COPY, commandAllocator)
    {
        SetDebugName(TEXT("CopyCommandList"));
    }
}