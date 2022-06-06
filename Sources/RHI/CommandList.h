#pragma once
#include <PCH.h>
#include <RHI/RHI.h>
#include <RHI/ResourceBarrier.h>

namespace sy::RHI
{
    /**
    * https://docs.microsoft.com/en-us/windows/win32/api/d3d12/nn-d3d12-id3d12graphicscommandlist
    * https://docs.microsoft.com/en-us/windows/win32/direct3d12/recording-command-lists-and-bundles  => Command list API Restrictions
    */
    class Device;
    class CommandAllocator;
    class DirectCommandAllocator;
    class BundleCommandAllocator;
    class ComputeCommandAllocator;
    class CopyCommandAllocator;
    class Resource;
    class Texture;
    class Buffer;
    class ResourceBarrier;
    class RTDescriptor;

    class CommandList : public RHIObject
    {
    public:
        void SetDebugName(const std::wstring_view debugName) override;

        void Reset();
        void Close();

        [[nodiscard]] ID3D12GraphicsCommandList6* D3DCommandList() const noexcept { return commandList.Get(); }
        [[nodiscard]] D3D12_COMMAND_LIST_TYPE Type() const noexcept { return type; }

        [[nodiscard]] class CopyCommandList& ToCopy() noexcept
        {
            assert(type == D3D12_COMMAND_LIST_TYPE_COPY);
            return reinterpret_cast<CopyCommandList&>(*this);
        }

        [[nodiscard]] class ComputeCommandList& ToCompute() noexcept
        {
            assert(type == D3D12_COMMAND_LIST_TYPE_COMPUTE);
            return reinterpret_cast<ComputeCommandList&>(*this);
        }

        [[nodiscard]] class DirectCommandList& ToDirect() noexcept
        {
            assert(type == D3D12_COMMAND_LIST_TYPE_DIRECT);
            return reinterpret_cast<DirectCommandList&>(*this);
        }

        [[nodiscard]] class BundleCommandList& ToBundle() noexcept
        {
            assert(type == D3D12_COMMAND_LIST_TYPE_BUNDLE);
            return reinterpret_cast<BundleCommandList&>(*this);
        }

    protected:
        CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type, const CommandAllocator& cmdAllocator);

    private:
        ComPtr<ID3D12GraphicsCommandList6> commandList;
        const CommandAllocator& cmdAllocator;
        const D3D12_COMMAND_LIST_TYPE type;

    };

    template <typename T>
    concept CommandListType = std::derived_from<T, CommandList>;

    class CopyCommandList : public CommandList
    {
    public:
        CopyCommandList(Device& device, const CopyCommandAllocator& commandAllocator);

        /** Proxy functions for Copy Command List */
        void CopyResource(const Resource& destination, const Resource& source);
        void AppendResourceBarrier(const ResourceBarrier& barrier);
        void AppendResourceBarriers(const ResourceBarrier::Vector_t& barriers);

    protected:
        using CommandList::CommandList;

    };

    /**
    * @brief    Compute = Copy + Compute
    */
    class ComputeCommandList : public CopyCommandList
    {
    public:
        ComputeCommandList(Device& device, const ComputeCommandAllocator& commandAllocator);

        /** Proxy functions for Compute Command List */

    protected:
        using CopyCommandList::CopyCommandList;

    };

    class DirectCommandListBase : public ComputeCommandList
    {
    public:
        /** Proxy functions for Direct Command List */

        /**
        * @brief Clear Render Target.
        */
        void ClearRenderTarget(const RTDescriptor& rtDescriptor, const DirectX::XMFLOAT4& color);

    protected:
        using ComputeCommandList::ComputeCommandList;

    };

    /**
    * @brief    Direct = Graphics + Compute + Copy
    */
    class DirectCommandList : public DirectCommandListBase
    {
    public:
        DirectCommandList(Device& device, const DirectCommandAllocator& commandAllocator);

    };

    /**
    * https://docs.microsoft.com/en-us/windows/win32/direct3d12/recording-command-lists-and-bundles
    */
    class BundleCommandList : public DirectCommandListBase
    {
    public:
        BundleCommandList(Device& device, const BundleCommandAllocator& commandAllocator);

    };
}