#pragma once
#include <vector>

#include "Vector.h"
#include "Shader.h"
#include "Math.h"

namespace Library
{
	class Renderer
	{
	private:
		int width, height;
		unsigned char* colorBuffer;
		float* depthBuffer;

		template<typename VertexOut> void LoadEdge(VertexOut& start, VertexOut& end, float* startInterpolants, float* endInterpolants) const
		{
			startInterpolants[0] = start.position[2];
			startInterpolants[1] = 1.0f;
			float* startData = reinterpret_cast<float*>(&start);
			std::memcpy(startInterpolants + 2, startData + 4, sizeof(VertexOut) - sizeof(Vector));
			endInterpolants[0] = end.position[2];
			endInterpolants[1] = 1.0f;
			float* endData = reinterpret_cast<float*>(&end);
			std::memcpy(endInterpolants + 2, endData + 4, sizeof(VertexOut) - sizeof(Vector));
			for (int i = 1; i < sizeof(VertexOut) / sizeof(float) - 2; i++)
			{
				startInterpolants[i] /= start.position[3];
				endInterpolants[i] /= end.position[3];
			}
		}

	public:
		Renderer(const int width, const int height);
		~Renderer();

		void Clear();

		void WriteToPointer(void* pointer);

		template<typename VertexIn, typename VertexOut, typename FragmentOut> void Draw(Shader<VertexIn, VertexOut, FragmentOut>& shader, std::vector<VertexIn>& vertices)
		{
			// Vertex Stage
			std::vector<VertexOut> vertexOutput;
			vertexOutput.reserve(vertices.size());
			for (int i = 0; i < vertices.size(); i++)
			{
				vertexOutput.push_back(shader.VertexMain(i, vertices[i]));
			}

			// Clipping Stage
			std::vector<VertexOut> clippingOutput;
			clippingOutput.reserve(vertexOutput.size());
			for (int i = 0; i + 2 < vertexOutput.size(); i += 3)
			{
				bool inFrustrum = true;
				for (int j = 0; j < 3; j++)
				{
					Vector clipPosition = vertexOutput[i + j].position;
					if (clipPosition[3] < 0) {
						inFrustrum = false;
						break;
					}
					float homogenizer = (1.0f / clipPosition[3]);
					clipPosition = clipPosition * homogenizer;
					for (int k = 0; k < 3; k++)
					{
						if (clipPosition[k] < -1.0f || clipPosition[k] > 1.0f)
						{
							inFrustrum = false;
							break;
						}
					}
					if (!inFrustrum)
						break;
				}
				if (inFrustrum)
				{
					for (int j = 0; j < 3; j++)
					{
						clippingOutput.push_back(vertexOutput[i + j]);
					}
				}
			}

			// Homogenization and Viewport Transform Stage
			std::vector<VertexOut> transformOutput;
			transformOutput.reserve(clippingOutput.size());
			for (int i = 0; i < clippingOutput.size(); i++)
			{
				VertexOut vertex = clippingOutput[i];
				float homogenizer = (1.0f / vertex.position[3]);
				vertex.position = vertex.position * homogenizer;
				vertex.position[0] = (vertex.position[0] + 1.0f) * width / 2.0f;
				vertex.position[1] = (vertex.position[1] + 1.0f) * height / 2.0f;
				vertex.position[3] = 1.0f / homogenizer;
				transformOutput.push_back(vertex);
			}

			// Rasterization Stage
			VertexOut* rasterizationOutput = (VertexOut*)malloc(sizeof(VertexOut) * 5000000);
			int fragmentSize = 0;
			int numberOfInterpolants = sizeof(VertexOut) / sizeof(float) - 2;
			float* interpolants = (float*)malloc(sizeof(float) * numberOfInterpolants);
			float* homogenizedInterpolants = (float*)malloc(sizeof(float) * numberOfInterpolants);
			float* deltaInterpolants = (float*)malloc(sizeof(float) * numberOfInterpolants);
			float* leftStartInterpolants = (float*)malloc(sizeof(float) * numberOfInterpolants);
			float* leftEndInterpolants = (float*)malloc(sizeof(float) * numberOfInterpolants);
			float* leftInterpolants = (float*)malloc(sizeof(float) * numberOfInterpolants);
			float* rightStartInterpolants = (float*)malloc(sizeof(float) * numberOfInterpolants);
			float* rightEndInterpolants = (float*)malloc(sizeof(float) * numberOfInterpolants);
			float* rightInterpolants = (float*)malloc(sizeof(float) * numberOfInterpolants);
			for (int i = 0; i < transformOutput.size(); i += 3)
			{
				VertexOut* vertices = &transformOutput[i];
				int top = 0;
				for (int i = 1; i < 3; i++)
				{
					if (vertices[i].position[1] < vertices[top].position[1])
					{
						top = i;
					}
				}
				Vector one = vertices[(top + 1) % 3].position;
				Vector two = vertices[(top + 2) % 3].position;
				Vector first = vertices[top].position;
				float nextY = std::fminf(one[1], two[1]);
				float oneParameter = Parameter(first[1], one[1], nextY);
				float twoParameter = Parameter(first[1], two[1], nextY);
				int left, right;
				if (Lerp(first[0], one[0], oneParameter) < Lerp(first[0], two[0], twoParameter))
				{
					left = (top + 1) % 3;
					right = (top + 2) % 3;
				}
				else
				{
					left = (top + 2) % 3;
					right = (top + 1) % 3;
				}

				float y = Round(first[1]) + 0.5f;
				VertexOut leftStart = vertices[top];
				VertexOut leftEnd = vertices[left];
				LoadEdge<VertexOut>(leftStart, leftEnd, leftStartInterpolants, leftEndInterpolants);
				VertexOut rightStart = vertices[top];
				VertexOut rightEnd = vertices[right];
				LoadEdge<VertexOut>(rightStart, rightEnd, rightStartInterpolants, rightEndInterpolants);
				while (y < leftEnd.position[1] || y < rightEnd.position[1])
				{
					if (y >= leftEnd.position[1])
					{
						leftStart = vertices[left];
						leftEnd = vertices[right];
						LoadEdge<VertexOut>(leftStart, leftEnd, leftStartInterpolants, leftEndInterpolants);
					}
					else if (y >= rightEnd.position[1])
					{
						rightStart = vertices[right];
						rightEnd = vertices[left];
						LoadEdge<VertexOut>(rightStart, rightEnd, rightStartInterpolants, rightEndInterpolants);
					}
					float leftParameter = Parameter(leftStart.position[1], leftEnd.position[1], y);
					float rightParameter = Parameter(rightStart.position[1], rightEnd.position[1], y);
					float leftX = Lerp(leftStart.position[0], leftEnd.position[0], leftParameter);
					float rightX = Lerp(rightStart.position[0], rightEnd.position[0], rightParameter);
					for (int j = 0; j < numberOfInterpolants; j++)
					{
						leftInterpolants[j] = Lerp(leftStartInterpolants[j], leftEndInterpolants[j], leftParameter);
						rightInterpolants[j] = Lerp(rightStartInterpolants[j], rightEndInterpolants[j], rightParameter);
						deltaInterpolants[j] = (rightInterpolants[j] - leftInterpolants[j]) / (rightX - leftX);
						interpolants[j] = leftInterpolants[j] + (Round(leftX) + 0.5f - leftX) * deltaInterpolants[j];
					}
					for (float x = Round(leftX) + 0.5f; x < rightX; x += 1.0f)
					{
						float parameter = Parameter(leftX, rightX, x);
						for (int j = 0; j < numberOfInterpolants; j++)
						{
							interpolants[j] += deltaInterpolants[j];
						}
						float w = 1.0f / interpolants[1];
						memcpy(homogenizedInterpolants, interpolants, sizeof(float) * numberOfInterpolants);
						for (int j = 1; j < numberOfInterpolants; j++)
						{
							homogenizedInterpolants[j] *= w;
						}
						VertexOut* fragment = &rasterizationOutput[fragmentSize++];
						fragment->position[0] = x;
						fragment->position[1] = y;
						fragment->position[2] = homogenizedInterpolants[0];
						fragment->position[3] = w;
						float* fragmentData = reinterpret_cast<float*>(fragment);
						std::memcpy(fragmentData + 4, homogenizedInterpolants + 2, sizeof(VertexOut) - sizeof(Vector));
					}
					y += 1.0f;
				}
			}
			free(interpolants);
			free(homogenizedInterpolants);
			free(deltaInterpolants);
			free(leftStartInterpolants);
			free(leftEndInterpolants);
			free(leftInterpolants);
			free(rightStartInterpolants);
			free(rightEndInterpolants);
			free(rightInterpolants);

			// Fragment and Framebuffer Stage
			FragmentOut output;
			unsigned char color[3];
			for (int i = 0; i < fragmentSize; i++)
			{
				float depth = rasterizationOutput[i].position[2];
				int row = (int)rasterizationOutput[i].position[1];
				int column = (int)rasterizationOutput[i].position[0];
				int offset = row * width + column;
				if (*(depthBuffer + offset) > depth)
				{
					shader.FragmentMain(rasterizationOutput[i], output);
					for (int i = 0; i < 3; i++)
					{
						color[i] = (unsigned char)(output.color[i] * 255.0f);
					}
					std::memcpy(colorBuffer + 3 * offset, color, sizeof(unsigned char) * 3);
					*(depthBuffer + offset) = depth;
				}
			}
			free(rasterizationOutput);
		}
	};
}