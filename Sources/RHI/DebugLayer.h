#pragma once
#include <PCH.h>
#include <Core/Exceptions.h>

namespace sy
{
	namespace DebugLayer
	{
		static void Enable()
		{
#if defined(_DEBUG) || defined(DEBUG)
			ComPtr<ID3D12Debug3> debugController;
			DXCall(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
			debugController->EnableDebugLayer();
#endif
		}
	}
}