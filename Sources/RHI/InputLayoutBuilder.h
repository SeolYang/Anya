#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy::RHI
{
    class InputLayoutBuilder final
    {
    public:
        [[nodiscard]] D3D12_INPUT_LAYOUT_DESC Build() const noexcept
        {
            ANYA_ASSERT(!elements.empty(), "Empty Input Layout!");
            return D3D12_INPUT_LAYOUT_DESC{
                .pInputElementDescs = elements.data(),
                .NumElements = static_cast<uint32>(elements.size())
            };
        }

        InputLayoutBuilder& AddPerVertexElement(const std::string_view semantic, const uint32 semanticIndex, const DXGI_FORMAT format, const uint32 inputSlot)
        {
            return AddElement(semantic, semanticIndex, format, inputSlot, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0);
        }

        InputLayoutBuilder& AddPerInstanceElement(const std::string_view semantic, const uint32 semanticIndex, const DXGI_FORMAT format, const uint32 inputSlot, const uint32 instanceDataStepRate)
        {
            return AddElement(semantic, semanticIndex, format, inputSlot, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, instanceDataStepRate);
        }

        InputLayoutBuilder& AddSinglePerVertexElement(const std::string_view semantic, const DXGI_FORMAT format, const uint32 inputSlot)
        {
            return AddPerVertexElement(semantic, 0, format, inputSlot);
        }

        InputLayoutBuilder& AddSinglePerInstanceElement(const std::string_view semantic, const DXGI_FORMAT format, const uint32 inputSlot, const uint32 instanceDataStepRate)
        {
            return AddPerInstanceElement(semantic, 0, format, inputSlot, instanceDataStepRate);
        }

    private:
        InputLayoutBuilder& AddElement(const std::string_view semantic, const uint32 semanticIndex, const DXGI_FORMAT format, const uint32 inputSlot, const D3D12_INPUT_CLASSIFICATION inputSlotClass, const uint32 instanceDataStepRate)
        {
            elements.emplace_back(semantic.data(), semanticIndex, format, inputSlot, currentOffset, inputSlotClass, instanceDataStepRate);
            currentOffset += SizeOfFormatInBytes(format);
            return *this;
        }

    private:
        std::vector<D3D12_INPUT_ELEMENT_DESC> elements = {};
        uint32 currentOffset = 0;

    };
}