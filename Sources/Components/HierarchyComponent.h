#pragma once
#include <Core/ECS.h>

namespace sy
{
	struct HierarchyComponent
	{
		Entity parentEntity = INVALID_ENTITY_HANDLE;
	};

	DeclareComponent(HierarchyComponent);
}