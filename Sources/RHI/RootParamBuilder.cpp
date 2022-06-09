#include <PCH.h>
#include <RHI/RootParamBuilder.h>
#include <Core/Assert.h>

namespace sy::RHI
{
    D3D12_ROOT_PARAMETER1 RootDescriptorTableBuilder::Build() const noexcept
    {
        D3D12_ROOT_PARAMETER1 builtParam;
        CD3DX12_ROOT_PARAMETER1::InitAsDescriptorTable(builtParam, static_cast<uint32>(ranges.size()), ranges.data(), GetShaderVisibility());
        return builtParam;
    }

    RootDescriptorTableBuilder& RootDescriptorTableBuilder::AddCBVDescriptorRange(uint32 numDescriptors, uint32 shaderRegister, uint32 registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, uint32 offsetInDescriptorsFromTableStart)
    {
        D3D12_DESCRIPTOR_RANGE1 range;
        CD3DX12_DESCRIPTOR_RANGE1::Init(range, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, numDescriptors, shaderRegister, registerSpace, flags, offsetInDescriptorsFromTableStart);
        return AddDescriptorRange(range);
    }

    RootDescriptorTableBuilder& RootDescriptorTableBuilder::AddSRVDescriptorRange(uint32 numDescriptors, uint32 shaderRegister, uint32 registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, uint32 offsetInDescriptorsFromTableStart)
    {
        D3D12_DESCRIPTOR_RANGE1 range;
        CD3DX12_DESCRIPTOR_RANGE1::Init(range, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, numDescriptors, shaderRegister, registerSpace, flags, offsetInDescriptorsFromTableStart);
        return AddDescriptorRange(range);
    }

    RootDescriptorTableBuilder& RootDescriptorTableBuilder::AddUAVDescriptorRange(uint32 numDescriptors, uint32 shaderRegister, uint32 registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, uint32 offsetInDescriptorsFromTableStart)
    {
        D3D12_DESCRIPTOR_RANGE1 range;
        CD3DX12_DESCRIPTOR_RANGE1::Init(range, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, numDescriptors, shaderRegister, registerSpace, flags, offsetInDescriptorsFromTableStart);
        return AddDescriptorRange(range);
    }

    RootDescriptorTableBuilder& RootDescriptorTableBuilder::AddSamplerDescriptorRange(uint32 numDescriptors, uint32 shaderRegister, uint32 registerSpace, D3D12_DESCRIPTOR_RANGE_FLAGS flags, uint32 offsetInDescriptorsFromTableStart)
    {
        D3D12_DESCRIPTOR_RANGE1 range;
        CD3DX12_DESCRIPTOR_RANGE1::Init(range, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, numDescriptors, shaderRegister, registerSpace, flags, offsetInDescriptorsFromTableStart);
        return AddDescriptorRange(range);
    }

    RootDescriptorTableBuilder& RootDescriptorTableBuilder::AddDescriptorRange(D3D12_DESCRIPTOR_RANGE1 range)
    {
        ranges.emplace_back(range);
        return *this;
    }

    D3D12_ROOT_PARAMETER1 RootDescriptorBuilder::Build() const noexcept
    {
        D3D12_ROOT_PARAMETER1 builtParam;

        switch (type)
        {
        case D3D12_ROOT_PARAMETER_TYPE_SRV:
            CD3DX12_ROOT_PARAMETER1::InitAsShaderResourceView(builtParam, shaderRegister, registerSpace, flags, GetShaderVisibility());
            break;

        case D3D12_ROOT_PARAMETER_TYPE_CBV:
            CD3DX12_ROOT_PARAMETER1::InitAsConstantBufferView(builtParam, shaderRegister, registerSpace, flags, GetShaderVisibility());
            break;

        case D3D12_ROOT_PARAMETER_TYPE_UAV:
            CD3DX12_ROOT_PARAMETER1::InitAsUnorderedAccessView(builtParam, shaderRegister, registerSpace, flags, GetShaderVisibility());
            break;

        default:
            ANYA_ASSERT(true, "Not supported RootDescriptor Type.");
            break;
        }

        return builtParam;
    }

    D3D12_ROOT_PARAMETER1 RootConstantBuilder::Build() const noexcept
    {
        D3D12_ROOT_PARAMETER1 builtParam;
        CD3DX12_ROOT_PARAMETER1::InitAsConstants(builtParam, num32BitValues, shaderRegister, registerSpace, GetShaderVisibility());
        return builtParam;
    }
}