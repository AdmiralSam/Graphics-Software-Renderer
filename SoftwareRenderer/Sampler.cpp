#include "Math.h"

#include "Sampler.h"

extern bool bilerp_flag;

using namespace std;

namespace Library
{
	Sampler::Sampler(Texture* texturePointer)
	{
		texture = texturePointer;
	}

	void Sampler::operator()(const float u, const float v, Vector& color) const
	{
		if (bilerp_flag)
		{
			float s = u * texture->width;
			float t = v * texture->height;
			int shiftedS = (int)(s - 0.5f);
			int shiftedT = (int)(t - 0.5f);
			int ds[] = { 0, 1, 0, 1 };
			int dt[] = { 0, 0, 1, 1 };
			float blitColors[4][3];
			for (int i = 0; i < 4; i++)
			{
				int blitS, blitT;
				WrapST(shiftedS + ds[i], shiftedT + dt[i], blitS, blitT);
				texture->getFloatColor(blitS, blitT, blitColors[i]);
			}
			float topColor[3];
			float bottomColor[3];
			float xParameter = Parameter(shiftedS + 0.5f, shiftedS + 1.5f, s);
			for (int i = 0; i < 3; i++)
			{
				topColor[i] = Lerp(blitColors[0][i], blitColors[1][i], xParameter);
				bottomColor[i] = Lerp(blitColors[2][i], blitColors[3][i], xParameter);
			}
			float yParameter = Parameter(shiftedT + 0.5f, shiftedT + 1.5f, t);
			for (int i = 0; i < 3; i++)
			{
				color[i] = Clamp(0.0f, 1.0f, Lerp(topColor[i], bottomColor[i], yParameter) / 255.0f);
			}
			color[3] = 1.0f;
		}
		else
		{
			int s = (int)(u * texture->width);
			int t = (int)(v * texture->height);
			WrapST(s, t, s, t);
			float textureColor[3];
			texture->getFloatColor(s, t, textureColor);
			for (int i = 0; i < 3; i++)
			{
				color[i] = textureColor[i] / 255.0f;
			}
			color[3] = 1.0f;
		}
	}
}