#pragma once
#include <PCH.h>
#include <Core/Assert.h>

namespace sy::RHI
{
    /** 
    * @brief    Default alignment for resources.
    *           = 64 KB (for Default Buffer and Texture Resource)
    *           https://asawicki.info/news_1726_secrets_of_direct3d_12_resource_alignment 
    */
    constexpr size_t GPU_DEFAULT_RESOURCE_ALIGNMENT = 65536;
    constexpr D3D12_RESOURCE_STATES D3D12_RESOURCE_STATE_BEFORE_INITIALIZE= static_cast<D3D12_RESOURCE_STATES>(0xffffffff);
    constexpr D3D12_RESOURCE_STATES D3D12_RESOURCE_STATE_UNKNOWN = static_cast<D3D12_RESOURCE_STATES>(0xfffffffe);

    class RHIObject
    {
    public:
        virtual ~RHIObject() noexcept = default;
        virtual void SetDebugName(const std::wstring_view debugName)
        { 
            this->debugName = debugName;
        }

        [[nodiscard]] std::wstring_view GetDebugName() const noexcept { return debugName; }

    protected:
        RHIObject() : 
            debugName(TEXT("RHIObject"))
        {
        }

    private:
        std::wstring debugName;

    };

    constexpr uint32 SizeOfFormatInBytes(const DXGI_FORMAT format)
    {
        switch (format)
        {
        default:
            ANYA_ASSERT(true, "Not supported Format.");
            return 0;

        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
        case DXGI_FORMAT_R32G32B32A32_UINT:
        case DXGI_FORMAT_R32G32B32A32_SINT:
            return 16;

        case DXGI_FORMAT_R32G32B32_TYPELESS:
        case DXGI_FORMAT_R32G32B32_FLOAT:
        case DXGI_FORMAT_R32G32B32_UINT:
        case DXGI_FORMAT_R32G32B32_SINT:
            return 12;

        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R16G16B16A16_UNORM:
        case DXGI_FORMAT_R16G16B16A16_UINT:
        case DXGI_FORMAT_R16G16B16A16_SNORM:
        case DXGI_FORMAT_R16G16B16A16_SINT:
        case DXGI_FORMAT_R32G32_TYPELESS:
        case DXGI_FORMAT_R32G32_FLOAT:
        case DXGI_FORMAT_R32G32_UINT:
        case DXGI_FORMAT_R32G32_SINT:
        case DXGI_FORMAT_R32G8X24_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            return 8;

        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
        case DXGI_FORMAT_R10G10B10A2_UINT:
        case DXGI_FORMAT_R11G11B10_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UINT:
        case DXGI_FORMAT_R8G8B8A8_SNORM:
        case DXGI_FORMAT_R8G8B8A8_SINT:
        case DXGI_FORMAT_R16G16_TYPELESS:
        case DXGI_FORMAT_R16G16_FLOAT:
        case DXGI_FORMAT_R16G16_UNORM:
        case DXGI_FORMAT_R16G16_UINT:
        case DXGI_FORMAT_R16G16_SNORM:
        case DXGI_FORMAT_R16G16_SINT:
        case DXGI_FORMAT_R32_TYPELESS:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_R32_UINT:
        case DXGI_FORMAT_R32_SINT:
        case DXGI_FORMAT_R24G8_TYPELESS:
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            return 4;

        case DXGI_FORMAT_R8G8_TYPELESS:
        case DXGI_FORMAT_R8G8_UNORM:
        case DXGI_FORMAT_R8G8_UINT:
        case DXGI_FORMAT_R8G8_SNORM:
        case DXGI_FORMAT_R8G8_SINT:
        case DXGI_FORMAT_R16_TYPELESS:
        case DXGI_FORMAT_R16_FLOAT:
        case DXGI_FORMAT_D16_UNORM:
        case DXGI_FORMAT_R16_UNORM:
        case DXGI_FORMAT_R16_UINT:
        case DXGI_FORMAT_R16_SNORM:
        case DXGI_FORMAT_R16_SINT:
            return 2;

        case DXGI_FORMAT_R8_TYPELESS:
        case DXGI_FORMAT_R8_UNORM:
        case DXGI_FORMAT_R8_UINT:
        case DXGI_FORMAT_R8_SNORM:
        case DXGI_FORMAT_R8_SINT:
        case DXGI_FORMAT_A8_UNORM:
            return 1;
        }
    }

    namespace StandardFormat
    {
        constexpr DXGI_FORMAT PositionFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        constexpr DXGI_FORMAT TextureCoordinateFormat = DXGI_FORMAT_R32G32_FLOAT;
        constexpr DXGI_FORMAT NormalFormat = DXGI_FORMAT_R32G32B32_FLOAT;
        constexpr DXGI_FORMAT TangentFormat = NormalFormat;
        constexpr DXGI_FORMAT BiTangentFormat = NormalFormat;
        constexpr DXGI_FORMAT VertexColorFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
    }

    namespace CheckFeatureSupport
    {
        static bool PresentAllowTearing()
        {
            static std::once_flag once;
            static bool result = false;

            std::call_once(once,
                []()
                {
                    BOOL allowTearing = FALSE;
                    ComPtr<IDXGIFactory4> factory4;
                    if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
                    {
                        ComPtr<IDXGIFactory5> factory5;
                        if (SUCCEEDED(factory4.As(&factory5)))
                        {
                            if (FAILED(factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))))
                            {
                                allowTearing = FALSE;
                            }
                        }
                    }

                    result = allowTearing == TRUE;
                }
            );

            return result;
        }
    }

    enum class EShaderType
    {
        Vertex,
        Hull,
        Domain,
        Geometry,
        Pixel,
        Compute,
        Amplification,
        Mesh
    };

    enum class EShaderModel
    {
        SM_6_0,
        SM_6_1,
        SM_6_2,
        SM_6_3,
        SM_6_4,
        SM_6_5,
        SM_6_6
    };
}

namespace sy
{
    inline std::wstring Stringify(const RHI::EShaderType type) noexcept
    {
        switch (type)
        {
        case RHI::EShaderType::Vertex:
            return TEXT("vs");
        case RHI::EShaderType::Hull:
            return TEXT("hs");
        case RHI::EShaderType::Domain:
            return TEXT("ds");
        case RHI::EShaderType::Geometry:
            return TEXT("gs");
        case RHI::EShaderType::Pixel:
            return TEXT("ps");
        case RHI::EShaderType::Compute:
            return TEXT("cs");
        case RHI::EShaderType::Amplification:
            return TEXT("as");
        case RHI::EShaderType::Mesh:
            return TEXT("ms");
        }

        assert(false && "Invalid Shader Type");
        return TEXT("Unknown");
    }

    inline std::wstring Stringify(const RHI::EShaderModel model) noexcept
    {
        switch (model)
        {
        case RHI::EShaderModel::SM_6_0:
            return TEXT("6_0");
        case RHI::EShaderModel::SM_6_1:
            return TEXT("6_1");
        case RHI::EShaderModel::SM_6_2:
            return TEXT("6_2");
        case RHI::EShaderModel::SM_6_3:
            return TEXT("6_3");
        case RHI::EShaderModel::SM_6_4:
            return TEXT("6_4");
        case RHI::EShaderModel::SM_6_5:
            return TEXT("6_5");
        case RHI::EShaderModel::SM_6_6:
            return TEXT("6_6");
        }

        assert(false && "Invalid Shader Model");
        return TEXT("Unknown");
    }

    inline std::wstring Stringify(const RHI::EShaderType type, const RHI::EShaderModel model)
    {
        return std::format(TEXT("{}_{}"), Stringify(type), Stringify(model));
    }
}
