#pragma once
#include <algorithm>

namespace Library
{
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
		if (difference < 0.00001f && -difference < 0.00001f)
			return 0.0f;
		return (value - start) / difference;
	}

#undef min
#undef max
	inline float Clamp(const float min, const float max, const float value)
	{
		return std::max(min, std::min(max, value));
	}
}