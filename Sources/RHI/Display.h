#pragma once
#include <PCH.h>
#include <Math/Math.h>

namespace sy
{
	/**
	* DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709 => REC709
	* DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020 => REC2020
	*/
	inline DXGI_FORMAT ConvertColorSpaceToColorFormatForBackbuffer(DXGI_COLOR_SPACE_TYPE colorSpace, bool bIsPreferHDR = false)
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

	class Display
	{
	public:
		Display(const DXGI_OUTPUT_DESC1& output);

		inline const auto& DesktopCoordinates() const { return desktopCoordinates; }
		inline const auto& RedPrimary() const { return redPrimary; }
		inline const auto& GreenPrimary() const { return greenPrimary; }
		inline const auto& BluePrimary() const { return bluePrimary; }
		inline const auto& WhitePoint() const { return whitePoint; }
		inline float MinLuminance() const { return minLuminance; }
		inline float MaxLuminance() const { return maxLuminance; }
		inline float FullFrameLuminance() const { return maxFullFrameLuminance; }
		inline DXGI_COLOR_SPACE_TYPE ColorSpace() const { return colorSpace; }
		inline bool IsHDRCapable() const { return bIsHDRCapable; }

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