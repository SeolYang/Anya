#include <PCH.h>
#include <RHI/GPURingBuffer.h>
#include <RHI/Device.h>
#include <Core/Exceptions.h>

namespace sy::RHI
{
    GPURingBuffer::GPURingBuffer(const Device& device, const size_t maxSizeOfBuffer, const bool bAllowCPUAccess) :
        ringBuffer{ maxSizeOfBuffer },
        cpuVirtualAddress(nullptr),
        gpuVirtualAddress(0),
        gpuBuffer(nullptr)
    {
        D3D12_HEAP_PROPERTIES heapProps;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        D3D12_RESOURCE_DESC resDesc;
        resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resDesc.Alignment = 0;
        resDesc.Height = 1;
        resDesc.DepthOrArraySize = 1;
        resDesc.MipLevels = 1;
        resDesc.Format = DXGI_FORMAT_UNKNOWN;
        resDesc.SampleDesc.Count = 1;
        resDesc.SampleDesc.Quality = 0;
        resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        D3D12_RESOURCE_STATES defaultUsage;
        if (bAllowCPUAccess)
        {
            heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
            resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            defaultUsage = D3D12_RESOURCE_STATE_GENERIC_READ;
        }
        else
        {
            heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
            resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
            defaultUsage = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        }
        resDesc.Width = maxSizeOfBuffer;

        DXCall(device.GetD3DDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, defaultUsage, nullptr, IID_PPV_ARGS(&gpuBuffer)));

        gpuVirtualAddress = gpuBuffer->GetGPUVirtualAddress();
        if (bAllowCPUAccess)
        {
            gpuBuffer->Map(0, nullptr, &cpuVirtualAddress);
        }

        SetDebugName(TEXT("Upload GPU RingBuffer"));
    }

    GPURingBuffer::GPURingBuffer(GPURingBuffer&& rhs) noexcept :
        ringBuffer(std::move(rhs.ringBuffer)),
        cpuVirtualAddress(std::exchange(rhs.cpuVirtualAddress, nullptr)),
        gpuVirtualAddress(std::exchange(rhs.gpuVirtualAddress, 0)),
        gpuBuffer(std::move(rhs.gpuBuffer))
    {
    }

    GPURingBuffer& GPURingBuffer::operator=(GPURingBuffer&& rhs) noexcept
    {
        ringBuffer = std::move(rhs.ringBuffer);
        cpuVirtualAddress = std::exchange(rhs.cpuVirtualAddress, nullptr);
        gpuVirtualAddress = std::exchange(rhs.gpuVirtualAddress, 0);
        gpuBuffer = std::move(rhs.gpuBuffer);

        return *this;
    }

    GPURingBuffer::DynamicAllocation GPURingBuffer::Allocate(const size_t sizeInBytes)
    {
        DynamicAllocation newAllocation{
            .Buffer = nullptr,
            .Offset = 0,
            .Size = 0
        };

        size_t offset = ringBuffer.Allocate(sizeInBytes);
        if (offset != RingBuffer::InvalidOffset)
        {
            newAllocation.Buffer = gpuBuffer.Get();
            newAllocation.Offset = offset;
            newAllocation.Size = sizeInBytes;
            newAllocation.GPUAddress = gpuVirtualAddress + offset;
            newAllocation.CPUAddress = cpuVirtualAddress;
            if (newAllocation.CPUAddress != nullptr)
            {
                newAllocation.CPUAddress = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(newAllocation.CPUAddress) + offset);
            }
        }

        return newAllocation;
    }

    void GPURingBuffer::SetDebugName(const std::wstring_view debugName)
    {
        RHIObject::SetDebugName(debugName);
        if (gpuBuffer != nullptr)
        {
            gpuBuffer->SetName(debugName.data());
        }
    }
}
