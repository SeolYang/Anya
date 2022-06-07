#include <PCH.h>
#include <RHI/CommandList.h>
#include <RHI/Device.h>
#include <RHI/CommandAllocator.h>
#include <RHI/Resource.h>
#include <RHI/Texture.h>
#include <RHI/ResourceBarrier.h>
#include <RHI/Descriptor.h>
#include <Core/Exceptions.h>
#include <Core/Assert.h>

namespace sy::RHI
{
    CommandList::CommandList(Device& device, D3D12_COMMAND_LIST_TYPE type, const CommandAllocator& cmdAllocator) :
        cmdAllocator(cmdAllocator),
        type(type),
        commandList([&]
        {
            ComPtr<ID3D12GraphicsCommandList7> cmdList;
            DXCall(device.GetD3DDevice()->CreateCommandList(device.GetNodeMask(), type, cmdAllocator.GetD3DCommandAllocator(), nullptr, IID_PPV_ARGS(&cmdList)));
            return cmdList;
        }())
    {
        
        /** Command List is on recording state. So close it at here to make able to call reset later. */
        this->Close();
    }

    CommandList::~CommandList()
    {
    }

    void CommandList::Open()
    {
        ANYA_ASSERT(commandList != nullptr, "D3D Command List was null.");
        DXCall(commandList->Reset(cmdAllocator.GetD3DCommandAllocator(), nullptr));
    }

    void CommandList::Close()
    {
        ANYA_ASSERT(commandList != nullptr, "D3D Command List was null.");
        DXCall(commandList->Close());
    }

    void CommandList::SetDebugName(const std::wstring_view debugName)
    {
        RHIObject::SetDebugName(debugName);
        if (commandList != nullptr)
        {
            DXCall(commandList->SetName(debugName.data()));
        }
    }

    CopyCommandList::CopyCommandList(Device& device, const CopyCommandAllocator& commandAllocator) :
        CommandList(device, D3D12_COMMAND_LIST_TYPE_COPY, commandAllocator)
    {
        SetDebugName(TEXT("CopyCommandList"));
    }

    void CopyCommandList::Open()
    {
        CommandList::Open();
        // resourceStateTracker.Reset();
        numCurrentBatchedBarriers = 0;
    }

    void CopyCommandList::Close()
    {
        //FlushResourceBarriers();
        CommandList::Close();
    }

    void CopyCommandList::CopyResource(const Resource& destination, const Resource& source)
    {
        assert((destination.GetD3DResource() != nullptr) && (source.GetD3DResource() != nullptr));
        GetD3DCommandList()->CopyResource(destination.GetD3DResource(), source.GetD3DResource());
    }

    void CopyCommandList::AppendResourceBarrier(const ResourceBarrier& barrier)
    {
        const auto targetBarrier = barrier.D3DResourceBarrier();
        GetD3DCommandList()->ResourceBarrier(1, &targetBarrier);
    }

    void CopyCommandList::AppendResourceBarriers(const ResourceBarrier::Vector_t& barriers)
    {
        const auto targetBarriers = ResourceBarriersToD3D(barriers);
        GetD3DCommandList()->ResourceBarrier((uint32)barriers.size(), targetBarriers.data());
    }

    ComputeCommandList::ComputeCommandList(Device& device, const ComputeCommandAllocator& commandAllocator) :
        CopyCommandList(device, D3D12_COMMAND_LIST_TYPE_COMPUTE, commandAllocator)
    {
        SetDebugName(TEXT("ComputeCommandList"));
    }

    void DirectCommandListBase::ClearRenderTarget(const RTDescriptor& rtDescriptor, const DirectX::XMFLOAT4& color)
    {
        GetD3DCommandList()->ClearRenderTargetView(rtDescriptor.GetCPUHandle(), &color.x, 0, nullptr);
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
}