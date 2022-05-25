#pragma once
#include <PCH.h>

namespace sy
{
	class ClearValue
	{
	public:
		ClearValue(DXGI_FORMAT format, DirectX::XMFLOAT4 color, float depth, uint8_t stencil) :
			format(format),
			color(color),
			depth(depth),
			stencil(stencil)
		{
		}

		inline D3D12_CLEAR_VALUE D3DClearValue() const
		{
			D3D12_CLEAR_VALUE output;
			output.Format = format;
			output.Color[0] = color.x;
			output.Color[1] = color.y;
			output.Color[2] = color.z;
			output.Color[3] = color.w;
			output.DepthStencil.Depth = depth;
			output.DepthStencil.Stencil = stencil;
			return output;
		}

	public:
		DXGI_FORMAT format;
		DirectX::XMFLOAT4 color = {0.0f, 0.0f, 0.0f, 0.0f};
		float depth = 1.0f;
		uint8_t stencil = 0;

	};
}
