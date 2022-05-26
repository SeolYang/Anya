#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy
{
    class Device;
    class CommandAllocator : public RHIObject
    {
    public:
       // CommandAllocator(const Device& device, )

    private:
        ComPtr<ID3D12CommandAllocator> commandAllocator;

    };
}