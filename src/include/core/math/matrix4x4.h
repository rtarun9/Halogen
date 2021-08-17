#pragma once

#include "../log.h"

#include <cmath>
#include <ostream>
#include <iostream>

namespace halogen::math
{
	class Matrix4x4
	{
	public:
		Matrix4x4();
		Matrix4x4(float value);
		Matrix4x4(float x, float y, float z, float w);
		Matrix4x4(float x0, float x1, float x2, float x3, float y0, float y1, float y2, float y3, float z0, float z1, float z2, float z3, float w0, float w1, float w2, float w3);

		~Matrix4x4() = default;

		Matrix4x4 add(const Matrix4x4& a, const Matrix4x4& b);
		Matrix4x4 subtract(const Matrix4x4& a, const Matrix4x4& b);
		Matrix4x4 multiply(const Matrix4x4& a, const Matrix4x4& b);

		Matrix4x4 operator+(const Matrix4x4& a);
		Matrix4x4 operator-(const Matrix4x4& a);
		Matrix4x4 operator*(const Matrix4x4& a);

		Matrix4x4 operator-();

		friend std::ostream& operator<<(std::ostream& out, Matrix4x4& mat);
		friend Matrix4x4 identity4();

	public:
		float  m_mat[4][4];
	};
}