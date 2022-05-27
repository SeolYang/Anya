#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <RHI/ResourceBarrier.h>

namespace sy
{
    /**
    * https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12graphicscommandlist
    */
    class Device;
    class CommandAllocator;
    class DirectCommandAllocator;
    class BundleCommandAllocator;
    class ComputeCommandAllocator;
    class CopyCommandAllocator;
    class RHIResource;
    class Texture;
    class Buffer;
    class ResourceBarrier;
    class RTDescriptor;

    class CommandList : public RHIObject
    {
    public:
        virtual void SetDebugName(const std::wstring_view debugName) override;

        void Reset();
        void Close();

        ID3D12GraphicsCommandList6* D3DCommandList() const noexcept { return commandList.Get(); }

    protected:
        CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type, CommandAllocator& commandAllocator);

    private:
        ComPtr<ID3D12GraphicsCommandList6> commandList;
        CommandAllocator& commandAllocator;

    };

    class CopyCommandListBase : public CommandList
    {
    public:
        /** Proxy functions for Copy Command List */
        void CopyResource(const RHIResource& destination, const RHIResource& source);
        void AppendResourceBarrier(const ResourceBarrier& barrier);
        void AppendResourceBarriers(const ResourceBarrier::Vector_t& barriers);

    protected:
        using CommandList::CommandList;

    };

    class ComputeCommandListBase : public CopyCommandListBase
    {
    public:
        /** Proxy functions for Compute Command List */

    protected:
        using CopyCommandListBase::CopyCommandListBase;

    };

    class DirectCommandListBase : public ComputeCommandListBase
    {
    public:
        /** Proxy functions for Direct Command List */

        /**
        * @brief Clear Render Target.
        */
        void ClearRenderTarget(const RTDescriptor& rtDescriptor, const DirectX::XMFLOAT4& color);

    protected:
        using ComputeCommandListBase::ComputeCommandListBase;

    };

    /**
    * @brief    Direct = Graphics + Compute + Copy
    */
    class DirectCommandList : public DirectCommandListBase
    {
    public:
        DirectCommandList(Device& device, DirectCommandAllocator& commandAllocator);

    };

    /**
    * https://docs.microsoft.com/en-us/windows/win32/direct3d12/recording-command-lists-and-bundles
    */
    class BundleCommandList : public DirectCommandListBase
    {
    public:
        BundleCommandList(Device& device, BundleCommandAllocator& commandAllocator);

    };

    /**
    * @brief    Compute = Copy + Compute
    */
    class ComputeCommandList : public ComputeCommandListBase
    {
    public:
        ComputeCommandList(Device& device, ComputeCommandAllocator& commandAllocator);

    };

    class CopyCommandList : public CopyCommandListBase
    {
    public:
        CopyCommandList(Device& device, CopyCommandAllocator& commandAllocator);

    };
}