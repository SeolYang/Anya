#include <PCH.h>
#include <RHI/Shader.h>

namespace sy::RHI
{
    Shader::Shader(ComPtr<IDxcBlob>&& shaderBlob, const EShaderType type) :
        shaderBlob(std::move(shaderBlob)),
        type(type)
    {
    }
}