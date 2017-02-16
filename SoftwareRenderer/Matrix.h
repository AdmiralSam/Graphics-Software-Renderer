#pragma once
#include <iostream>

namespace Library
{
	class Vector;
	class Matrix
	{
	public:
		static Matrix Identity();
		static Matrix Translation(const float x, const float y, const float z);
		static Matrix RotationX(const float angle);
		static Matrix RotationY(const float angle);
		static Matrix RotationZ(const float angle);
		static Matrix Perspective(const float fovy, const float aspectRatio, const float near, const float far);
		
		Matrix();

		float& operator()(const int row, const int column);

		friend Matrix operator*(Matrix& a, Matrix& b);
		friend Vector operator*(Matrix& a, Vector& b);

		friend std::ostream& operator<<(std::ostream& stream, Matrix& matrix);

	private:
		float data[16];
	};
}