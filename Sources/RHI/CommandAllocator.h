#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy
{
    class Device;
    class CommandAllocator : public RHIObject
    {
    public:
        CommandAllocator(Device& device, D3D12_COMMAND_LIST_TYPE commandListType);
        
        virtual void SetDebugName(const std::wstring_view debugName) override;

        auto D3DCommandAllocator() const { return commandAllocator.Get(); }

    private:
        ComPtr<ID3D12CommandAllocator> commandAllocator;

    };

    class DirectCommandAllocator : public CommandAllocator
    {
    public:
        DirectCommandAllocator(Device& device);

    };

    class BundleCommandAllocator : public CommandAllocator
    {
    public:
        BundleCommandAllocator(Device& device);

    };

    class ComputeCommandAllocator : public CommandAllocator
    {
    public:
        ComputeCommandAllocator(Device& device);

    };

    class CopyCommandAllocator : public CommandAllocator
    {
    public:
        CopyCommandAllocator(Device& device);

    };
}