#pragma once
#include <PCH.h>
#include <Core/ECS.h>

namespace sy
{
    struct VisibleToRendererTag
    {
    };

    DeclareComponent(VisibleToRendererTag);
}