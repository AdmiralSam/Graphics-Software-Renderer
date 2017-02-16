#include <cmath>

#include "Vector.h"
#include "Matrix.h"

using namespace std;

namespace Library
{
	Matrix Matrix::Identity()
	{
		Matrix a;
		for (int i = 0; i < 4; i++)
		{
			a(i, i) = 1.0f;
		}
		return a;
	}

	Matrix Matrix::Translation(const float x, const float y, const float z)
	{
		Matrix translation = Identity();
		translation(0, 3) = x;
		translation(1, 3) = y;
		translation(2, 3) = z;
		return translation;
	}

	Matrix Matrix::RotationX(const float angle)
	{
		Matrix rotationX = Identity();
		rotationX(1, 1) = cos(angle);
		rotationX(1, 2) = -sin(angle);
		rotationX(2, 1) = sin(angle);
		rotationX(2, 2) = cos(angle);
		return rotationX;
	}

	Matrix Matrix::RotationY(const float angle)
	{
		Matrix rotationY = Identity();
		rotationY(0, 0) = cos(angle);
		rotationY(0, 2) = sin(angle);
		rotationY(2, 0) = -sin(angle);
		rotationY(2, 2) = cos(angle);
		return rotationY;
	}

	Matrix Matrix::RotationZ(const float angle)
	{
		Matrix rotationZ = Identity();
		rotationZ(0, 0) = cos(angle);
		rotationZ(0, 1) = -sin(angle);
		rotationZ(1, 0) = sin(angle);
		rotationZ(1, 1) = cos(angle);
		return rotationZ;
	}

	Matrix Matrix::Perspective(const float fovy, const float aspectRatio, const float near, const float far)
	{
		Matrix projection;
		float top = near * tanf(fovy / 2.0f);
		float right = aspectRatio * top;
		projection = Matrix();
		projection(0, 0) = near / right;
		projection(1, 1) = near / top;
		projection(2, 2) = (far + near) / (near - far);
		projection(2, 3) = -2.0f * far * near / (far - near);
		projection(3, 2) = -1.0f;
		return projection;
	}

	Matrix::Matrix()
	{
		for (int i = 0; i < 16; i++)
		{
			data[i] = 0.0f;
		}
	}

	float& Matrix::operator()(const int row, const int column)
	{
		return data[row * 4 + column];
	}

	Matrix operator*(Matrix& a, Matrix& b)
	{
		Matrix c;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					c(i, j) += a(i, k) * b(k, j);
				}
			}
		}
		return c;
	}

	ostream& operator<<(ostream& stream, Matrix& matrix)
	{
		for (int i = 0; i < 4; i++)
		{
			stream << "\n| ";
			for (int j = 0; j < 4; j++)
			{
				stream << matrix(i, j) << " ";
			}
			stream << "|";
		}
		return stream;
	}
}