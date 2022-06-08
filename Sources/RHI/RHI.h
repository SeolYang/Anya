#pragma once
#include <PCH.h>

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

}
