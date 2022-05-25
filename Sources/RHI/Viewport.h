#pragma once
#include <PCH.h>

namespace sy
{
	class Viewport
	{
	public:
		Viewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth);

		inline auto D3DViewport() const
		{
			return D3D12_VIEWPORT{
				.TopLeftX = topLeftX,
				.TopLeftY = topLeftY,
				.Width = width,
				.Height = height,
				.MinDepth = minDepth,
				.MaxDepth = maxDepth
			};
		}

	public:
		float topLeftX = 0.0f;
		float topLeftY = 0.0f;
		float width = 1.0f;
		float height = 1.0f;
		float minDepth = 0.0f;
		float maxDepth = 1.0f;

	};
}