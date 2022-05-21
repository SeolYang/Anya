#pragma once
#include <PCH.h>
#include <Core/ECS.h>

namespace sy
{
    struct TransformComponent : Component
    {
        DirectX::XMFLOAT3 LocalPosition = DirectX::XMFLOAT3();
        DirectX::XMFLOAT3 LocalScale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
        DirectX::XMFLOAT4 LocalRotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    };

    DeclareComponent(TransformComponent);
}