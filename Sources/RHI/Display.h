#pragma once
#include <PCH.h>
#include <Math/Math.h>
#include <RHI/RHI.h>

namespace sy::RHI
{
	/**
	* DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709 => REC709
	* DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 => REC2020
	*/
	inline DXGI_FORMAT ConvertColorSpaceToColorFormatForBackbuffer(DXGI_COLOR_SPACE_TYPE colorSpace, bool bIsPreferHDR = false)  noexcept
	{
		switch (colorSpace)
		{
		default:
		case DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709:
			return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

		case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020:
			return (bIsPreferHDR ? DXGI_FORMAT::DXGI_FORMAT_R10G10B10A2_UNORM : DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
		}
	}

	class Display : public RHIObject
	{
	public:
		Display(const DXGI_OUTPUT_DESC1& output) noexcept;

		[[nodiscard]] const auto& GetDesktopCoordinates() const noexcept { return desktopCoordinates; }
		[[nodiscard]] const auto& GetRedPrimary() const noexcept { return redPrimary; }
		[[nodiscard]] const auto& GetGreenPrimary() const noexcept { return greenPrimary; }
		[[nodiscard]] const auto& GetBluePrimary() const noexcept { return bluePrimary; }
		[[nodiscard]] const auto& GetWhitePoint() const noexcept { return whitePoint; }
		[[nodiscard]] float GetMinLuminance() const noexcept { return minLuminance; }
		[[nodiscard]] float GetMaxLuminance() const noexcept { return maxLuminance; }
		[[nodiscard]] float GetFullFrameLuminance() const noexcept { return maxFullFrameLuminance; }
		[[nodiscard]] DXGI_COLOR_SPACE_TYPE GetColorSpace() const noexcept { return colorSpace; }
		[[nodiscard]] bool IsHDRCapable() const noexcept { return bIsHDRCapable; }

	private:
		RECT desktopCoordinates;
		DirectX::XMFLOAT2 redPrimary;
		DirectX::XMFLOAT2 greenPrimary;
		DirectX::XMFLOAT2 bluePrimary;
		DirectX::XMFLOAT2 whitePoint;
		float minLuminance;
		float maxLuminance;
		float maxFullFrameLuminance;
		DXGI_COLOR_SPACE_TYPE colorSpace;
		bool bIsHDRCapable;

	};
}