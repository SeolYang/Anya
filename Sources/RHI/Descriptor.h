#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy
{
    class CPUDescriptor
    {
    public:
        CPUDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE handle) noexcept :
            cpuHandle(handle)
        {
        }

        virtual ~CPUDescriptor() noexcept = 0;

        const D3D12_CPU_DESCRIPTOR_HANDLE& CPUHandle() const noexcept { return cpuHandle; }
        const auto& CPUAddress() const noexcept { return cpuHandle.ptr; }

    private:
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;

    };

    class GPUDescriptor : public CPUDescriptor
    {
    public:
        GPUDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) noexcept :
            CPUDescriptor(cpuHandle),
            gpuHandle(gpuHandle)
        {
        }

        virtual ~GPUDescriptor() noexcept = 0;

    private:
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;

    };

    class RTDescriptor : public CPUDescriptor
    {
    public:
        using CPUDescriptor::CPUDescriptor;
        ~RTDescriptor() noexcept = default;
    };
}