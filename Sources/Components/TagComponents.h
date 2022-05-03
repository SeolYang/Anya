#pragma once
#include <PCH.h>
#include <Core/ECS.h>

namespace anya
{
    struct VisibleToRendererTag
    {
    };

    DeclareComponent(VisibleToRendererTag);
}