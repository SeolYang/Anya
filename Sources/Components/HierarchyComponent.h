#pragma once
#include <Core/ECS.h>

namespace sy
{
	struct HierarchyComponent : Component
	{
	public:
		HierarchyComponent(const Entity parent = INVALID_ENTITY_HANDLE) :
			Parent(parent)
		{
		}

	public:
		Entity Parent;

	};

	DeclareComponent(HierarchyComponent);
}