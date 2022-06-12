#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

namespace sy::RHI
{
    class Shader : public RHIObject
    {
    public:
        Shader(ComPtr<IDxcBlob>&& shaderBlob, EShaderType type);

        [[nodiscard]] EShaderType GetType() const noexcept { return type; }

    private:
        ComPtr<IDxcBlob> shaderBlob;
        EShaderType type;
    };
}