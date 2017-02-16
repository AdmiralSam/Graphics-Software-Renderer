#pragma once
#include <stdlib.h>

#include "Vector.h"

namespace Library
{
	struct VertexInput {};

	struct VertexOutput
	{
		Vector position;
	};

	struct FragmentOutput
	{
		Vector color;
	};

	template<typename VertexIn, typename VertexOut, typename FragmentOut> class Shader
	{
	public:
		virtual VertexOut VertexMain(int id, VertexIn& input) = 0;
		virtual void FragmentMain(VertexOut& input, FragmentOut& output) = 0;

	protected:
		Shader()
		{
			free(VertexInputConstraint);
			free(VertexOutputConstraint);
			free(FragmentOutputConstraint);
		}

	private:
		VertexInput* VertexInputConstraint = new VertexIn();
		VertexOutput* VertexOutputConstraint = new VertexOut();
		FragmentOutput* FragmentOutputConstraint = new FragmentOut();
	};
}