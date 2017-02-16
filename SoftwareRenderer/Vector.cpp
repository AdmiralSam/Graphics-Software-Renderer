#include <iostream>

#include "Matrix.h"
#include "Vector.h"

using namespace std;

namespace Library
{
	Vector Vector::Position(const float x, const float y, const float z)
	{
		Vector a;
		a[0] = x;
		a[1] = y;
		a[2] = z;
		a[3] = 1.0f;
		return a;
	}

	Vector::Vector()
	{
		for (int i = 0; i < 4; i++)
		{
			data[i] = 0.0f;
		}
	}

	float& Vector::operator[](const int index)
	{
		return data[index];
	}

	Vector operator*(float& a, Vector& b)
	{
		Vector c;
		for (int i = 0; i < 4; i++)
		{
			c[i] = a * b[i];
		}
		return c;
	}

	Vector operator*(Vector& a, float& b)
	{
		return operator*(b, a);
	}

	Vector operator*(Matrix& a, Vector& b)
	{
		Vector c;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				c[i] += a(i, j) * b[j];
			}
		}
		return c;
	}

	ostream& operator<<(ostream& stream, Vector& vector)
	{
		return stream << "(" << vector[0] << ", " << vector[1] << ", " << vector[2] << ", " << vector[3] << ")";
	}
}