#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy::RHI
{
    class Device;
    /** Command list can executed immediately after it has been excuted on CMD Queue. */
    /** But, Command allocator can't be reusable until commands stored in the cmd allocator have finished executing on the cmd queue. */
    class CommandAllocator : public RHIObject
    {
    public:
        virtual void SetDebugName(const std::wstring_view debugName) override;

        void Reset() { commandAllocator->Reset(); }

        auto D3DCommandAllocator() const { return commandAllocator.Get(); }

    protected:
        CommandAllocator(Device& device, D3D12_COMMAND_LIST_TYPE commandListType);

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