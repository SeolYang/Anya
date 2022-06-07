#include <PCH.h>
#include <RHI/CommandAllocator.h>
#include <RHI/Device.h>
#include <Core/Exceptions.h>

namespace sy::RHI
{
    CommandAllocator::CommandAllocator(Device& device, D3D12_COMMAND_LIST_TYPE commandListType)
    {
        DXCall(device.GetD3DDevice()->CreateCommandAllocator(commandListType, IID_PPV_ARGS(&commandAllocator)));
        SetDebugName(TEXT("CommandAllocator"));
    }

    void CommandAllocator::SetDebugName(const std::wstring_view debugName)
    {
        RHIObject::SetDebugName(debugName);
        if (commandAllocator != nullptr)
        {
            DXCall(commandAllocator->SetName(debugName.data()));
        }
    }

    DirectCommandAllocator::DirectCommandAllocator(Device& device) :
        CommandAllocator(device, D3D12_COMMAND_LIST_TYPE_DIRECT)
    {
        SetDebugName(TEXT("DirectCommandAllocator"));
    }

    BundleCommandAllocator::BundleCommandAllocator(Device& device) :
        CommandAllocator(device, D3D12_COMMAND_LIST_TYPE_BUNDLE)
    {
        SetDebugName(TEXT("BundleCommandAllocator"));
    }

    ComputeCommandAllocator::ComputeCommandAllocator(Device& device) :
        CommandAllocator(device, D3D12_COMMAND_LIST_TYPE_COMPUTE)
    {
        SetDebugName(TEXT("ComputeCommandAllocator"));
    }

    CopyCommandAllocator::CopyCommandAllocator(Device& device) :
        CommandAllocator(device, D3D12_COMMAND_LIST_TYPE_COPY)
    {
        SetDebugName(TEXT("CopyCommandAllocator"));
    }
}