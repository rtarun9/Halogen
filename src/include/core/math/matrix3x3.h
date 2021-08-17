#pragma once

#include "../log.h"

#include <cmath>
#include <ostream>
#include <iostream>

namespace halogen::math
{
	class Matrix3x3
	{
	public:
		Matrix3x3();
		Matrix3x3(float value);
		Matrix3x3(float x, float y, float z);
		Matrix3x3(float x0, float x1, float x2, float y0, float y1, float y2, float z0, float z1, float z2);

		~Matrix3x3() = default;

		Matrix3x3 add(const Matrix3x3& a, const Matrix3x3& b);
		Matrix3x3 subtract(const Matrix3x3& a, const Matrix3x3& b);
		Matrix3x3 multiply(const Matrix3x3& a, const Matrix3x3& b);

		Matrix3x3 operator+(const Matrix3x3& a);
		Matrix3x3 operator-(const Matrix3x3& a);
		Matrix3x3 operator*(const Matrix3x3& a);

		Matrix3x3 operator-();

		friend std::ostream& operator<<(std::ostream& out, Matrix3x3& mat);

		friend Matrix3x3 identity3();

	public:
		float m_mat[3][3];
	};

	using Mat3x3f = Matrix3x3;
}