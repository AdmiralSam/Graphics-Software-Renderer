#pragma once
#include "Matrix.h"
#include "Vector.h"
#include "Shader.h"

struct CheckeredIn : Library::VertexInput
{
	Library::Vector position;
	float u;
	float v;
};

struct CheckeredVertexOut : Library::VertexOutput
{
	float u;
	float v;
};

struct CheckeredOut : Library::FragmentOutput
{
};

class CheckeredShader : public Library::Shader<CheckeredIn, CheckeredVertexOut, CheckeredOut>
{
public:
	CheckeredShader() : Library::Shader<CheckeredIn, CheckeredVertexOut, CheckeredOut>() {}

	Library::Matrix modelViewProjection;

	virtual CheckeredVertexOut VertexMain(int id, CheckeredIn& input);
	virtual void FragmentMain(CheckeredVertexOut& input, CheckeredOut& output);
};