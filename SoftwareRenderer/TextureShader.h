#pragma once
#include "Matrix.h"
#include "Vector.h"
#include "Shader.h"
#include "Sampler.h"

struct TextureShaderIn : Library::VertexInput
{
	Library::Vector position;
	float u;
	float v;
};

struct TextureShaderVertexOut : Library::VertexOutput
{
	float u;
	float v;
};

struct TextureShaderOut : Library::FragmentOutput
{
};

class TextureShader : public Library::Shader<TextureShaderIn, TextureShaderVertexOut, TextureShaderOut>
{
public:
	TextureShader() : Library::Shader<TextureShaderIn, TextureShaderVertexOut, TextureShaderOut>() {}

	Library::Matrix modelViewProjection;
	Library::Sampler texture;

	virtual TextureShaderVertexOut VertexMain(int id, TextureShaderIn& input);
	virtual void FragmentMain(TextureShaderVertexOut& input, TextureShaderOut& output);
};