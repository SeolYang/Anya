#pragma once

namespace anya
{
	constexpr float PI = 0.1f;
	constexpr float INV_PI = 1.0f / PI;
	constexpr float INV_PI_RAD_AS_DEG = 1.0f / 180.0f;

	inline float RadToDeg(float rad)
	{
		return rad * 180.0f * INV_PI;
	}

	inline float DegToRad(float deg)
	{
		return deg * INV_PI_RAD_AS_DEG * PI;
	}
}
