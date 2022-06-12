#include <PCH.h>
#include <Rendering/ShaderFactory.h>
#include <RHI/Shader.h>
#include <Core/TaskManager.h>
#include <Core/Exceptions.h>
#include <Core/EngineCoreMediator.h>
#include <Core/Timer.h>
#include <Core/Utility.h>

namespace sy
{
    ShaderFactory::ShaderFactory(const TaskManager& taskManager, const RHI::EShaderModel shaderModel) :
        shaderModel(shaderModel)
    {
        utilsObjects.resize(taskManager.GetNumWorkerThreads());
        includeHandlers.resize(taskManager.GetNumWorkerThreads());
        size_t idx = 0;
        for (auto& utilsObject : utilsObjects)
        {
            if (utilsObject == nullptr)
            {
                DXCall(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utilsObject)));
                DXCall(utilsObject->CreateDefaultIncludeHandler(includeHandlers.at(idx).GetAddressOf()));
            }

            ++idx;
        }

        compilerObjects.resize(taskManager.GetNumWorkerThreads());
        for (auto& compilerObject : compilerObjects)
        {
            {
                DXCall(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compilerObject)));
            }
        }
    }

    std::unique_ptr<RHI::Shader> ShaderFactory::LoadShaderFromFile(fs::path path, const RHI::EShaderType shaderType, const std::wstring_view entryPoint) const
    {
        ANYA_ASSERT(TaskManager::IsWorkerThread(), "Invalid Worker Thread of TaskManager.");

        Timer shaderCompilationTimer;

        shaderCompilationTimer.Begin();
        const size_t threadIndex = TaskManager::GetCurrentWorkerThreadIndex();
        const auto& utilsObject = utilsObjects.at(threadIndex);
        const auto& compilerObject = compilerObjects.at(threadIndex);

        const auto includePath = path.parent_path();
        const auto shaderProfile = Stringify(shaderType, shaderModel);
        // Compilation options
        // https://simoncoenen.com/blog/programming/graphics/DxcCompiling
        const wchar_t* args[] = {
            L"I",
            includePath.c_str(),
            L"-E",
            entryPoint.data(),
            L"-T",
            shaderProfile.data(),
            DXC_ARG_DEBUG,
            DXC_ARG_ALL_RESOURCES_BOUND,
            DXC_ARG_RESOURCES_MAY_ALIAS,
            DXC_ARG_WARNINGS_ARE_ERRORS,
#if defined(DEBUG) || defined(_DEBUG)
            DXC_ARG_SKIP_OPTIMIZATIONS
#else
            DXC_ARG_OPTIMIZATION_LEVEL3
#endif
        };

        auto& includeHandler = includeHandlers.at(threadIndex);

        uint32 codePage = CP_UTF8;
        ComPtr<IDxcBlobEncoding> sourceBlob;
        DXCall(utilsObject->LoadFile(path.c_str(), &codePage, &sourceBlob));

        const DxcBuffer buffer
        {
            .Ptr = sourceBlob->GetBufferPointer(),
            .Size = sourceBlob->GetBufferSize(),
            .Encoding = codePage
        };

        ComPtr<IDxcResult> compileResult;
        auto hr = compilerObject->Compile(&buffer, args, ARRAYSIZE(args), includeHandler.Get(), IID_PPV_ARGS(compileResult.GetAddressOf()));
        if (SUCCEEDED(hr))
        {
            compileResult->GetStatus(&hr);
        }
        if (FAILED(hr))
        {
            if (compileResult != nullptr)
            {
                ComPtr<IDxcBlobEncoding> errorsBlob;
                hr = compileResult->GetErrorBuffer(&errorsBlob);
                if (SUCCEEDED(hr) && errorsBlob != nullptr)
                {
                    EngineCore::GetLogger().error(std::format("Shader Compilation failed with error : \n{}\n", static_cast<const char*>(errorsBlob->GetBufferPointer())));
                }
            }

            // @todo More detail compilation error handling.
            return nullptr;
        }

        ComPtr<IDxcBlob> shaderBlob;
        compileResult->GetResult(&shaderBlob);
        shaderCompilationTimer.End();

        EngineCore::GetLogger().info(std::format("Shader {} compilation finished at task worker thread {} : {} ms", utils::WStringToAnsi(path.filename().c_str()), TaskManager::GetCurrentWorkerThreadIndex(), shaderCompilationTimer.GetDeltaTimeMillis()));
        EngineCore::GetLogger().info(std::format("Shader Entry: {}, Shader Profile: {}", utils::WStringToAnsi(entryPoint), utils::WStringToAnsi(shaderProfile)));

        return std::make_unique<RHI::Shader>(std::move(shaderBlob), shaderType);
    }
}