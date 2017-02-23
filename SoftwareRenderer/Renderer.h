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

		template<typename VertexOut> void LoadClippingEdge(VertexOut& in, VertexOut& out, float* inInterpolants, float* outInterpolants) const
		{
			float* startData = reinterpret_cast<float*>(&in);
			std::memcpy(inInterpolants, startData, sizeof(VertexOut));
			float* endData = reinterpret_cast<float*>(&out);
			std::memcpy(outInterpolants, endData, sizeof(VertexOut));
		}

		inline float CalculateClippingParameter(const float inValue, const float inW, const float outValue, const float outW, const float side)
		{
			float numerator = side * inW - inValue;
			float denominator = (outValue - inValue) - side * (outW - inW);
			if (std::fabsf(denominator) < Epsilon)
			{
				return 0.0f;
			}
			return numerator / denominator;
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
			std::vector<VertexOut> clippingOutput, clippingBuffer;
			clippingBuffer = vertexOutput;
			clippingOutput.reserve(vertexOutput.size());

			float* insideInterpolants = (float*)malloc(sizeof(VertexOut));
			float* outsideInterpolants = (float*)malloc(sizeof(VertexOut));
			float* clippedInterpolants = (float*)malloc(sizeof(VertexOut));

			bool bufferIsOutput = false;
			std::vector<int> inside, outside;
			for (int dimension = 0; dimension < 3; dimension++)
			{
				for (float side = -1.0f; side <= 1.0f; side += 2.0f)
				{
					vector<VertexOut>& input = bufferIsOutput ? clippingOutput : clippingBuffer;
					vector<VertexOut>& output = bufferIsOutput ? clippingBuffer : clippingOutput;
					output.clear();
					for (int i = 0; i + 2 < input.size(); i += 3)
					{
						inside.clear();
						outside.clear();
						for (int j = 0; j < 3; j++)
						{
							if (side * input[i + j].position[dimension] > input[i + j].position[3])
							{
								outside.push_back(j);
							}
							else
							{
								inside.push_back(j);
							}
						}
						switch (inside.size())
						{
						case 3:
							for (int j = 0; j < 3; j++)
							{
								output.push_back(input[i + j]);
							}
							break;
						case 2:
						{
							VertexOut& inVertex1 = input[i + inside[0]];
							VertexOut& inVertex2 = input[i + inside[1]];
							VertexOut& outVertex = input[i + outside[0]];
							VertexOut clippedVertices1[3];
							clippedVertices1[inside[0]] = inVertex1;
							clippedVertices1[inside[1]] = inVertex2;
							LoadClippingEdge<VertexOut>(inVertex1, outVertex, insideInterpolants, outsideInterpolants);
							float parameter = CalculateClippingParameter(inVertex1.position[dimension], inVertex1.position[3], outVertex.position[dimension], outVertex.position[3], side);
							for (int j = 0; j < sizeof(VertexOut) / sizeof(float); j++)
							{
								clippedInterpolants[j] = Lerp(insideInterpolants[j], outsideInterpolants[j], parameter);
							}
							std::memcpy(&clippedVertices1[outside[0]], clippedInterpolants, sizeof(VertexOut));
							for (int j = 0; j < 3; j++)
							{
								output.push_back(clippedVertices1[j]);
							}

							VertexOut clippedVertices2[3];
							clippedVertices2[inside[1]] = inVertex2;
							std::memcpy(&clippedVertices2[inside[0]], clippedInterpolants, sizeof(VertexOut));
							LoadClippingEdge<VertexOut>(inVertex2, outVertex, insideInterpolants, outsideInterpolants);
							parameter = CalculateClippingParameter(inVertex2.position[dimension], inVertex2.position[3], outVertex.position[dimension], outVertex.position[3], side);
							for (int j = 0; j < sizeof(VertexOut) / sizeof(float); j++)
							{
								clippedInterpolants[j] = Lerp(insideInterpolants[j], outsideInterpolants[j], parameter);
							}
							std::memcpy(&clippedVertices2[outside[0]], clippedInterpolants, sizeof(VertexOut));
							for (int j = 0; j < 3; j++)
							{
								output.push_back(clippedVertices2[j]);
							}
						};
						break;
						case 1:
						{
							VertexOut& inVertex = input[i + inside[0]];
							VertexOut clippedVertices[3];
							clippedVertices[inside[0]] = inVertex;
							for (int j = 0; j < 2; j++)
							{
								VertexOut& outVertex = input[i + outside[j]];
								LoadClippingEdge<VertexOut>(inVertex, outVertex, insideInterpolants, outsideInterpolants);
								float parameter = CalculateClippingParameter(inVertex.position[dimension], inVertex.position[3], outVertex.position[dimension], outVertex.position[3], side);
								for (int k = 0; k < sizeof(VertexOut) / sizeof(float); k++)
								{
									clippedInterpolants[k] = Lerp(insideInterpolants[k], outsideInterpolants[k], parameter);
								}
								std::memcpy(&clippedVertices[outside[j]], clippedInterpolants, sizeof(VertexOut));
							}
							for (int j = 0; j < 3; j++)
							{
								output.push_back(clippedVertices[j]);
							}
						};
						break;
						default:
							break;
						}
					}
					bufferIsOutput = !bufferIsOutput;
				}
			}
			clippingOutput.clear();
			for (int i = 0; i < clippingBuffer.size(); i++)
			{
				if (clippingBuffer[i].position[3] >= 0.0f)
				{
					clippingOutput.push_back(clippingBuffer[i]);
				}
			}
			free(insideInterpolants);
			free(outsideInterpolants);
			free(clippedInterpolants);

			// Homogenization and Viewport Transform Stage
			std::vector<VertexOut> transformOutput;
			transformOutput.reserve(clippingOutput.size());
			for (int i = 0; i < clippingOutput.size(); i++)
			{
				VertexOut vertex = clippingOutput[i];
				float homogenizer = (1.0f / vertex.position[3]);
				vertex.position = vertex.position * homogenizer;
				vertex.position[0] = Clamp(0.0f, window_width, (vertex.position[0] + 1.0f) * width / 2.0f);
				vertex.position[1] = Clamp(0.0f, window_height, (vertex.position[1] + 1.0f) * height / 2.0f);
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
				Vector& one = vertices[(top + 1) % 3].position;
				Vector& two = vertices[(top + 2) % 3].position;
				Vector& first = vertices[top].position;
				int left, right;
				if (CrossProduct2D(one[0] - first[0], one[1] - first[1], two[0] - first[0], two[1] - first[1]) <= 0.0f)
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
						std::memcpy(homogenizedInterpolants, interpolants, sizeof(float) * numberOfInterpolants);
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