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

        DirectX::XMMATRIX LocalMatrix() const
        {
            /** SRT(Pre-multiplication with row vector) */
            DirectX::XMVECTOR localPosition { };
            DirectX::XMLoadFloat3(&LocalPosition);
            DirectX::XMVECTOR localScale { };
            DirectX::XMLoadFloat3(&LocalScale);
            DirectX::XMVECTOR localRotation { };
            DirectX::XMLoadFloat4(&LocalRotation);

            auto S = DirectX::XMMatrixScalingFromVector(localScale);
            auto R = DirectX::XMMatrixRotationQuaternion(localRotation);
            auto T = DirectX::XMMatrixTranslationFromVector(localPosition);
            return DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(S, R), T);
        }

        DirectX::XMMATRIX WorldMatrix(DirectX::XMMATRIX parentWorldMatrix) const
        {
            return DirectX::XMMatrixMultiply(parentWorldMatrix, LocalMatrix());
        }
    };

    DeclareComponent(TransformComponent);
}