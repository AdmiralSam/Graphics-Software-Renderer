#include "TextureShader.h"

using namespace std;
using namespace Library;

TextureShaderVertexOut TextureShader::VertexMain(int id, TextureShaderIn& input)
{
	TextureShaderVertexOut output;
	output.position = modelViewProjection * input.position;
	output.u = input.u;
	output.v = input.v;
	return output;
}

void TextureShader::FragmentMain(TextureShaderVertexOut& input, TextureShaderOut& output)
{
	texture(input.u, input.v, output.color);
}