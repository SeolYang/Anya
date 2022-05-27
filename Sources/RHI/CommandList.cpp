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
        SetDebugName(TEXT("CommandList"));
    }

    void CommandList::SetDebugName(const std::wstring_view debugName)
    {
        RHIObject::SetDebugName(debugName);
        if (commandList != nullptr)
        {
            commandList->SetName(debugName.data());
        }
    }

    void CommandList::Reset()
    {
        if (commandList != nullptr)
        {
            commandList->Reset(commandAllocator.D3DCommandAllocator(), nullptr);
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