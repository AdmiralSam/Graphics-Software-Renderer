#include "cstdlib"

#include "CheckeredShader.h"

using namespace std;
using namespace Library;

CheckeredVertexOut CheckeredShader::VertexMain(int id, CheckeredIn& input)
{
	CheckeredVertexOut output;
	output.position = modelViewProjection * input.position;
	output.u = input.u;
	output.v = input.v;
	return output;
}

void CheckeredShader::FragmentMain(CheckeredVertexOut& input, CheckeredOut& output)
{
	int row = (int)(input.v * 8);
	int column = (int)(input.u * 8);
	for (int i = 0; i < 3; i++)
	{
		output.color[i] = (row + column) % 2 == 0 ? 0.0f : 1.0f;
	}
	output.color[3] = 1.0f;
}