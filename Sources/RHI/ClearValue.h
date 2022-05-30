#pragma once
#include <PCH.h>

namespace sy::RHI
{
	class ClearValue
	{
	public:
		ClearValue(DXGI_FORMAT format, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), float depth = 1.0f, uint8_t stencil = 0) :
			Format(format),
			Color(color),
			Depth(std::clamp(depth, D3D12_MIN_DEPTH, D3D12_MAX_DEPTH)),
			Stencil(stencil)
		{
		}

		inline D3D12_CLEAR_VALUE D3DClearValue() const
		{
			D3D12_CLEAR_VALUE output;
			output.Format = Format;
			output.Color[0] = Color.x;
			output.Color[1] = Color.y;
			output.Color[2] = Color.z;
			output.Color[3] = Color.w;
			output.DepthStencil.Depth = Depth;
			output.DepthStencil.Stencil = Stencil;
			return output;
		}

	public:
		DXGI_FORMAT Format;
		DirectX::XMFLOAT4 Color;
		float Depth;
		uint8_t Stencil;

	};
}
