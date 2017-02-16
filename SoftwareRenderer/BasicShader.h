#pragma once
#include "Matrix.h"
#include "Vector.h"
#include "Shader.h"

struct BasicIn : Library::VertexInput
{
	Library::Vector position;
};

struct BasicVertexOut : Library::VertexOutput
{
	Library::Vector color;
};

struct BasicOut : Library::FragmentOutput
{
};

class BasicShader : public Library::Shader<BasicIn, BasicVertexOut, BasicOut>
{
public:
	BasicShader() : Library::Shader<BasicIn, BasicVertexOut, BasicOut>() {}

	Library::Matrix modelViewProjection;

	virtual BasicVertexOut VertexMain(int id, BasicIn& input);
	virtual void FragmentMain(BasicVertexOut& input, BasicOut& output);
};