#include <vector>

#include "Renderer.h"

using namespace std;

namespace Library
{
	Renderer::Renderer(const int width, const int height)
	{
		this->width = width;
		this->height = height;
		colorBuffer = (unsigned char*)malloc(sizeof(unsigned char) * 3 * width * height);
		depthBuffer = (float*)malloc(sizeof(float) * width * height);
	}

	Renderer::~Renderer()
	{
		free(colorBuffer);
		free(depthBuffer);
	}

	void Renderer::Clear()
	{
		memset(colorBuffer, 0, sizeof(unsigned char) * 3 * width * height);
		for (int i = 0; i < width * height; i++)
		{
			depthBuffer[i] = 1.0f;
		}
	}

	void Renderer::WriteToPointer(void* pointer)
	{
		memcpy(pointer, colorBuffer, sizeof(unsigned char) * 3 * width * height);
	}
}