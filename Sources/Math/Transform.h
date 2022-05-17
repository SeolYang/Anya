#pragma once
#include <PCH.h>

namespace sy
{
    class Transform
    {
    private:
        DirectX::XMFLOAT3 localPosition;
        DirectX::XMFLOAT3 localScale;
        DirectX::XMFLOAT4 localRotation;

    };
}