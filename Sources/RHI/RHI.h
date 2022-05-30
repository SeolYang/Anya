#pragma once
#include <PCH.h>

namespace sy::RHI
{
    class RHIObject
    {
    public:
        virtual ~RHIObject() noexcept = default;
        virtual void SetDebugName(const std::wstring_view debugName)
        { 
            this->debugName = debugName;
        }

        std::wstring_view DebugName() const noexcept { return debugName; }

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
