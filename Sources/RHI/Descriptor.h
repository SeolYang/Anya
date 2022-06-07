#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy::RHI
{
    /* https://www.geeksforgeeks.org/pure-virtual-destructor-c/ */
    class CPUDescriptor
    {
    public:
        CPUDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE handle) noexcept :
            cpuHandle(handle)
        {
        }

        CPUDescriptor() noexcept :
            cpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE())
        {
        }

        virtual ~CPUDescriptor() noexcept = 0;

        [[nodiscard]] const D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHandle() const noexcept { return cpuHandle; }
        [[nodiscard]] const auto& GetCPUAddress() const noexcept { return cpuHandle.ptr; }

    protected:
        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;

    };

    template <typename T>
    concept DescriptorType = std::is_base_of_v<CPUDescriptor, T>;

    class GPUDescriptor : public CPUDescriptor
    {
    public:
        GPUDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) noexcept :
            CPUDescriptor(cpuHandle),
            gpuHandle(gpuHandle)
        {
        }

        GPUDescriptor() noexcept : GPUDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE(), D3D12_GPU_DESCRIPTOR_HANDLE())
        {
        }

        virtual ~GPUDescriptor() noexcept = 0;

    private:
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;

    };

    template <typename T>
    concept GPUDescriptorType = std::is_base_of_v<GPUDescriptor, T>;

    /**
    * @brief    Constant Buffer Descriptor
    */
    class CBDescriptor : public GPUDescriptor
    {
    public:
        using GPUDescriptor::GPUDescriptor;
        ~CBDescriptor() noexcept = default;

    };

    /**
    * @brief    Shader Resource Descriptor
    */
    class SRDescriptor : public GPUDescriptor
    {
    public:
        using GPUDescriptor::GPUDescriptor;
        ~SRDescriptor() noexcept = default;

    };

    /**
    * @brief    Unordered Access Descriptor
    */
    class UADescriptor : public GPUDescriptor
    {
    public:
        using GPUDescriptor::GPUDescriptor;
        ~UADescriptor() noexcept = default;

    };

    /**
    * @brief    (Dynamic) Sampler Descriptor
    */
    class SamplerDescriptor : public GPUDescriptor
    {
    public:
        using GPUDescriptor::GPUDescriptor;
        ~SamplerDescriptor() noexcept = default;

    };

    /**
    * @brief    Render Target Descriptor
    */
    class RTDescriptor : public CPUDescriptor
    {
    public:
        using CPUDescriptor::CPUDescriptor;
        ~RTDescriptor() noexcept = default;

    };

    /**
    * @brief    Depth Stencil Descriptor
    */
    class DSDescriptor : public CPUDescriptor
    {
    public:
        using CPUDescriptor::CPUDescriptor;
        ~DSDescriptor() noexcept = default;

    };
}