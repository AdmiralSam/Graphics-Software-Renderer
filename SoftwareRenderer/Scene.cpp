#include <iostream>
#include <vector>
#include <map>

#include "Vector.h"
#include "Matrix.h"
#include "BasicShader.h"
#include "CheckeredShader.h"
#include "TextureShader.h"
#include "Renderer.h"

#include "Scene.h"

#define DegreesToRadians 2.0f * 3.1415f / 360.0f

using namespace std;
using namespace Library;

extern FrameBuffer fb;
map<Texture*, vector<Triangle*>> trianglesPerTexture;
Matrix projection;
Matrix modelView;
Renderer renderer(window_width, window_height);

/* set the perspective projection matrix given the following values */
void setPerspectiveProjection(float eye_fov, float aspect_ratio, float near_plane, float far_plane) {
	projection = Matrix::Perspective(eye_fov * DegreesToRadians, aspect_ratio, near_plane, far_plane);
}

/* set the modelview matrix with the given parameters */
void setModelviewMatrix(float *eye_pos, float eye_theta, float eye_phi) {
	Matrix phi = Matrix::RotationX(-eye_phi * DegreesToRadians);
	Matrix theta = Matrix::RotationZ(-eye_theta * DegreesToRadians);
	Matrix translate = Matrix::Translation(-eye_pos[0], -eye_pos[1], -eye_pos[2]);
	modelView = phi * theta * translate;
}

/* this implements the software rendering of the scene */ 
void Scene::renderSceneSoftware(void) {
	if (trianglesPerTexture.size() == 0)
	{
		for (TriangleList* current = original_head; current != NULL; current = current->next)
		{
			if (trianglesPerTexture.find(current->t->tex) == trianglesPerTexture.end())
				trianglesPerTexture[current->t->tex] = vector<Triangle*>();
			trianglesPerTexture[current->t->tex].push_back(current->t);
		}
	}
	TextureShader shader;
	shader.modelViewProjection = projection * modelView;
	renderer.Clear();
	for (const auto& pair : trianglesPerTexture)
	{
		shader.texture = Sampler(pair.first);
		vector<TextureShaderIn> list;
		for (const auto& triangle : pair.second)
		{
			for (int i = 0; i < 3; i++)
			{
				TextureShaderIn vertex;
				memcpy(&vertex.position, &triangle->v[i], sizeof(float) * 4);
				vertex.u = triangle->coords[i][0];
				vertex.v = triangle->coords[i][1];
				list.push_back(vertex);
			}
		}
		renderer.Draw<TextureShaderIn, TextureShaderVertexOut, TextureShaderOut>(shader, list);
	}
	renderer.WriteToPointer(fb.getColorPtr(0, 0));
	/*
	BasicShader shader;
	shader.modelViewProjection = projection * modelView;
	vector<BasicIn> list;
	for (TriangleList* current = original_head; current != NULL; current = current->next)
	{
		for (int i = 0; i < 3; i++)
		{
			BasicIn vertex;
			memcpy(&vertex.position, &current->t->v[i], sizeof(float) * 4);
			list.push_back(vertex);
		}
	}

	renderer.Clear();
	renderer.Draw<BasicIn, BasicVertexOut, BasicOut>(shader, list);
	renderer.WriteToPointer(fb.getColorPtr(0, 0));
	*/
	/*
	CheckeredShader shader;
	shader.modelViewProjection = projection * modelView;
	vector<CheckeredIn> list;
	for (TriangleList* current = original_head; current != NULL; current = current->next)
	{
		for (int i = 0; i < 3; i++)
		{
			CheckeredIn vertex;
			memcpy(&vertex.position, &current->t->v[i], sizeof(float) * 4);
			vertex.u = current->t->coords[i][0];
			vertex.v = current->t->coords[i][1];
			list.push_back(vertex);
		}
	}
	renderer.Clear();
	renderer.Draw<CheckeredIn, CheckeredVertexOut, CheckeredOut>(shader, list);
	renderer.WriteToPointer(fb.getColorPtr(0, 0));
	*/
}