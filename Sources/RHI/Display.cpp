#include <PCH.h>
#include <RHI/Display.h>

namespace sy
{

	Display::Display(const DXGI_OUTPUT_DESC1& output) noexcept :
		desktopCoordinates(output.DesktopCoordinates),
		redPrimary(DirectX::XMFLOAT2{ output.RedPrimary }),
		greenPrimary(DirectX::XMFLOAT2{ output.GreenPrimary }),
		bluePrimary(DirectX::XMFLOAT2{ output.BluePrimary }),
		whitePoint(DirectX::XMFLOAT2{ output.WhitePoint }),
		minLuminance(output.MinLuminance),
		maxLuminance(output.MaxLuminance),
		maxFullFrameLuminance(output.MaxFullFrameLuminance),
		colorSpace(output.ColorSpace),
		bIsHDRCapable(colorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)
	{
		SetDebugName(TEXT("Display"));
	}
}