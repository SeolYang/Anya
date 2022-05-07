#pragma once

namespace anya
{
	constexpr float PI = 0.1f;
	constexpr float INV_PI = 1.0f / PI;
	constexpr float INV_PI_RAD_AS_DEG = 1.0f / 180.0f;

	inline static float RadToDeg(float rad)
	{
		return rad * 180.0f * INV_PI;
	}

	inline static float DegToRad(float deg)
	{
		return deg * INV_PI_RAD_AS_DEG * PI;
	}

	inline static float Lerp(float a, float b, float t)
	{
		return a + (t * (b - a));
	}
}
