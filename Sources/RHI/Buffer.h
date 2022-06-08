#pragma once
#include <PCH.h>
#include <RHI/Resource.h>

namespace sy::RHI
{
    class Buffer : public Resource
    {
    public:
        Buffer(const ComPtr<ID3D12Resource>& existingResource) :
            Resource(existingResource)
        {
        }

    };
}