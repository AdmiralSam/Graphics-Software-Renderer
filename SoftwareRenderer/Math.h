#pragma once
#include <algorithm>

namespace Library
{
	const float Epsilon = 0.001f;

	inline int Round(const float number)
	{
		bool roundDown = number - (int)number <= 0.5f;
		return roundDown ? (int)number : ((int)number + 1);
	}

	inline float Lerp(const float start, const float end, const float parameter)
	{
		return (1.0f - parameter) * start + parameter * end;
	}

	inline float Parameter(const float start, const float end, const float value)
	{
		float difference = end - start;
		if (std::fabsf(difference) < 0.00001f)
			return 0.0f;
		return (value - start) / difference;
	}

	inline float CrossProduct2D(const float x1, const float y1, const float x2, const float y2)
	{
		return x1 * y2 - y1 * x2;
	}

#undef min
#undef max
	inline float Clamp(const float min, const float max, const float value)
	{
		return std::max(min, std::min(max, value));
	}
}