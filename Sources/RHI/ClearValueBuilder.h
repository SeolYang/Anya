#pragma once
#include <PCH.h>

namespace sy::RHI
{
	class ClearValueBuilder
	{
	public:
		ClearValueBuilder(DXGI_FORMAT format, SimpleMath::Color color = SimpleMath::Color(), float depth = 1.0f, uint8_t stencil = 0) :
			Format(format),
			Color(color),
			Depth(std::clamp(depth, D3D12_MIN_DEPTH, D3D12_MAX_DEPTH)),
			Stencil(stencil)
		{
		}

		[[nodiscard]] D3D12_CLEAR_VALUE Build() const noexcept
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

		ClearValueBuilder& SetFormat(const DXGI_FORMAT format) noexcept
		{
			Format = format;
			return *this;
		}

		ClearValueBuilder& SetColor(const SimpleMath::Color& color)
		{
			Color = color;
			return *this;
		}

		ClearValueBuilder& SetDepth(const float depth)
		{
			Depth = std::clamp(depth, D3D12_MIN_DEPTH, D3D12_MAX_DEPTH);
			return *this;
		}

		ClearValueBuilder& SetStencil(const uint8 stencil)
		{
			Stencil = stencil;
			return *this;
		}

	private:
		DXGI_FORMAT Format;
		SimpleMath::Color Color;
		float Depth;
		uint8 Stencil;

	};
}
