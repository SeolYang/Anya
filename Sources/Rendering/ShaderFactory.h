#pragma once
#include <PCH.h>
#include <RHI/RHI.h>

// https://asawicki.info/news_1719_two_shader_compilers_of_direct3d_12
// https://simoncoenen.com/blog/programming/graphics/DxcCompiling
namespace sy
{

    namespace RHI
    {
        class Shader;
    }

    class TaskManager;
    class ShaderFactory final
    {
    public:
        ShaderFactory(const TaskManager& taskManager, RHI::EShaderModel shaderModel = RHI::EShaderModel::SM_6_5);
        ~ShaderFactory() = default;

        ShaderFactory(const ShaderFactory&) = delete;
        ShaderFactory(ShaderFactory&&) noexcept = delete;
        ShaderFactory& operator=(const ShaderFactory&) = delete;
        ShaderFactory& operator=(ShaderFactory&&) noexcept = delete;

        std::unique_ptr<RHI::Shader> LoadShaderFromFile(fs::path path, const RHI::EShaderType shaderType, std::wstring_view entryPoint) const;

    private:
        const RHI::EShaderModel shaderModel;
        std::vector<ComPtr<IDxcUtils>> utilsObjects;
        std::vector<ComPtr<IDxcCompiler3>> compilerObjects;
        std::vector<ComPtr<IDxcIncludeHandler>> includeHandlers;

    };
}