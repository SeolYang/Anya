#pragma once
#include <PCH.h>

namespace sy
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
}
