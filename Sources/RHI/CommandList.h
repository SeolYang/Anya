#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy
{
    class Device;
    class CommandAllocator;
    class DirectCommandAllocator;
    class BundleCommandAllocator;
    class ComputeCommandAllocator;
    class CopyCommandAllocator;

    class CommandList : public RHIObject
    {
    public:
        virtual void SetDebugName(const std::wstring_view debugName) override;

        void Reset();
        void Close();

        ID3D12GraphicsCommandList* D3DCommandList() const noexcept { return commandList.Get(); }

    protected:
        CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type, CommandAllocator& commandAllocator);

    private:
        ComPtr<ID3D12GraphicsCommandList> commandList;
        CommandAllocator& commandAllocator;

    };

    class DirectCommandList : public CommandList
    {
    public:
        DirectCommandList(Device& device, DirectCommandAllocator& commandAllocator);

    };

    class BundleCommandList : public CommandList
    {
    public:
        BundleCommandList(Device& device, BundleCommandAllocator& commandAllocator);

    };

    class ComputeCommandList : public CommandList
    {
    public:
        ComputeCommandList(Device& device, ComputeCommandAllocator& commandAllocator);

    };

    class CopyCommandList : public CommandList
    {
    public:
        CopyCommandList(Device& device, CopyCommandAllocator& commandAllocator);

    };
}