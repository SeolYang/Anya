#pragma once
#include <PCH.h>
#include <Core/Exceptions.h>

#define FORCE_DISABLE_DEBUG_LAYER 0

namespace sy
{
	namespace DebugLayer
	{
		static void Enable()
		{
#if (defined(_DEBUG) || defined(DEBUG)) && !FORCE_DISABLE_DEBUG_LAYER
			ComPtr<ID3D12Debug3> debugController;
			DXCall(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
#endif
		}
	}
}