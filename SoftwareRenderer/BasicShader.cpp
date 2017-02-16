#include "cstdlib"

#include "BasicShader.h"

using namespace std;
using namespace Library;

BasicVertexOut BasicShader::VertexMain(int id, BasicIn& input)
{
	int triangleId = id;
	srand(triangleId + 280);
	rand();
	BasicVertexOut output;
	output.position = modelViewProjection * input.position;
	output.color[0] = (float)rand() / RAND_MAX;
	output.color[1] = (float)rand() / RAND_MAX;
	output.color[2] = (float)rand() / RAND_MAX;
	output.color[3] = 1.0f;
	return output;
}

void BasicShader::FragmentMain(BasicVertexOut& input, BasicOut& output)
{
	output.color = input.color;
}