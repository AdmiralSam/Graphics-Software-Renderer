#pragma once
#include "Texture.h"
#include "Vector.h"

namespace Library
{
	class Sampler
	{
	public:
		Sampler() {}
		Sampler(Texture* texturePointer);

		void operator()(const float u, const float v, Vector& color) const;

	private:
		Texture* texture;

		inline void WrapST(const int s, const int t, int& wrappedS, int& wrappedT) const
		{
			wrappedS = s % texture->width;
			wrappedT = t % texture->height;
			if (wrappedS < 0)
			{
				wrappedS += texture->width;
			}
			if (wrappedT < 0)
			{
				wrappedT += texture->height;
			}
		}
	};
}