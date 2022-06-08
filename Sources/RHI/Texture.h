#pragma once
#include <PCH.h>
#include <RHI/Resource.h>

namespace sy::RHI
{
    class Texture : public Resource
    {
    public:
        Texture(const ComPtr<ID3D12Resource>& existingResource) :
            Resource(existingResource)
        {
        }

        /** @TODO   Load Texture from external file. */

  
    };
}