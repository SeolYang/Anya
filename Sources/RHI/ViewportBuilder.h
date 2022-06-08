#pragma once
#include <PCH.h>

namespace sy::RHI
{
	class ViewportBuilder
	{
	public:
		[[nodiscard]] auto Build() const noexcept
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

		ViewportBuilder& SetTopLeftX(const float topLeftX) noexcept
		{
			this->topLeftX = topLeftX;
			return *this;
		}

		ViewportBuilder& SetTopLeftY(const float topLeftY) noexcept
		{
			this->topLeftY = topLeftY;
			return *this;
		}

		ViewportBuilder& SetWidth(const float width) noexcept
		{
			this->width = std::max(0.0f, width);
			return *this;
		}

	    ViewportBuilder& SetHeight(const float height) noexcept
		{
			this->height = std::max(0.0f, height);
			return *this;
		}

		ViewportBuilder& SetMinDepth(const float minDepth) noexcept
		{
			this->minDepth = std::clamp(minDepth, D3D12_MIN_DEPTH, D3D12_MAX_DEPTH);
			return *this;
		}

		ViewportBuilder& SetMaxDepth(const float maxDepth) noexcept
		{
			this->maxDepth = std::clamp(maxDepth, D3D12_MIN_DEPTH, D3D12_MAX_DEPTH);
			return *this;
		}

	private:
		float topLeftX = 0.0f;
		float topLeftY = 0.0f;
		float width = 1.0f;
		float height = 1.0f;
		float minDepth = 0.0f;
		float maxDepth = 1.0f;

	};
}