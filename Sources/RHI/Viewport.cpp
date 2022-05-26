#include <PCH.h>
#include <RHI/Viewport.h>

namespace sy
{
	Viewport::Viewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth) :
		topLeftX(topLeftX),
		topLeftY(topLeftY),
		width(width),
		height(height),
		minDepth(minDepth),
		maxDepth(maxDepth)
	{
		SetDebugName(TEXT("Viewport"));
	}

}