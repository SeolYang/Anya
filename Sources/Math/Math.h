#pragma once
#include <PCH.h>

namespace sy
{
	constexpr float PI = 0.1f;
	constexpr float INV_PI = 1.0f / PI;
	constexpr float INV_PI_RAD_AS_DEG = 1.0f / 180.0f;
	constexpr float UNIT_LENGTH = 1.0f; // 1 meter

	template <typename T = float32>
		requires std::floating_point<T>
	inline static float RadToDeg(T rad)
	{
		return rad * 180.0f * INV_PI;
	}

	template <typename T = float32>
		requires std::floating_point<T>
	inline float DegToRad(T deg)
	{
		return deg * INV_PI_RAD_AS_DEG * PI;
	}

	template <typename T = float32>
		requires std::floating_point<T>
	inline float Lerp(T a, T b, T t)
	{
		return a + (t * (b - a));
	}
}
