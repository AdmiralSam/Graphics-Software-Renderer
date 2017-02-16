#pragma once
#include <iostream>

namespace Library
{
	class Matrix;
	class Vector
	{
	public:
		static Vector Position(const float x, const float y, const float z);

		Vector();

		float& operator[](const int index);

		friend Vector operator*(float& a, Vector& b);
		friend Vector operator*(Vector& a, float& b);
		friend Vector operator*(Matrix& a, Vector& b);

		friend std::ostream& operator<<(std::ostream& stream, Vector& vector);

	private:
		float data[4];
	};
}