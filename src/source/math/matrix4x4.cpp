#include "../../include/core/math/matrix4x4.h"

namespace halogen::math
{

	Matrix4x4::Matrix4x4()
	{
		m_mat[0][0] = 0.0f;
		m_mat[0][1] = 0.0f;
		m_mat[0][2] = 0.0f;
		m_mat[0][3] = 0.0f;
		m_mat[1][0] = 0.0f;
		m_mat[1][1] = 0.0f;
		m_mat[1][2] = 0.0f;
		m_mat[1][3] = 0.0f;
		m_mat[2][0] = 0.0f;
		m_mat[2][1] = 0.0f;
		m_mat[2][2] = 0.0f;
		m_mat[2][3] = 0.0f;
		m_mat[3][0] = 0.0f;
		m_mat[3][1] = 0.0f;
		m_mat[3][2] = 0.0f;
		m_mat[3][3] = 0.0f;
	}

	Matrix4x4::Matrix4x4(float value)
	{
		m_mat[0][0] = value;
		m_mat[0][1] = 0.0f;
		m_mat[0][2] = 0.0f;
		m_mat[0][3] = 0.0f;
		m_mat[1][0] = 0.0f;
		m_mat[1][1] = value;
		m_mat[1][2] = 0.0f;
		m_mat[1][3] = 0.0f;
		m_mat[2][0] = 0.0f;
		m_mat[2][1] = 0.0f;
		m_mat[2][2] = value;
		m_mat[2][3] = 0.0f;
		m_mat[3][0] = 0.0f;
		m_mat[3][1] = 0.0f;
		m_mat[3][2] = 0.0f;
		m_mat[3][3] = value;
	}

	Matrix4x4::Matrix4x4(float x, float y, float z, float w)
	{
		m_mat[0][0] = x;
		m_mat[0][1] = 0.0f;
		m_mat[0][2] = 0.0f;
		m_mat[0][3] = 0.0f;
		m_mat[1][0] = 0.0f;
		m_mat[1][1] = y;
		m_mat[1][2] = 0.0f;
		m_mat[1][3] = 0.0f;
		m_mat[2][0] = 0.0f;
		m_mat[2][1] = 0.0f;
		m_mat[2][2] = z;
		m_mat[2][3] = 0.0f;
		m_mat[3][0] = 0.0f;
		m_mat[3][1] = 0.0f;
		m_mat[3][2] = 0.0f;
		m_mat[3][3] = w;
	}

	Matrix4x4::Matrix4x4(float x0, float x1, float x2, float x3, float y0, float y1, float y2, float y3, float z0, float z1, float z2, float z3, float w0, float w1, float w2, float w3)
	{
		m_mat[0][0] = x0;
		m_mat[0][1] = x1;
		m_mat[0][2] = x2;
		m_mat[0][3] = x3;
		m_mat[1][0] = y0;
		m_mat[1][1] = y1;
		m_mat[1][2] = y2;
		m_mat[1][3] = y3;
		m_mat[2][0] = z0;
		m_mat[2][1] = z1;
		m_mat[2][2] = z2;
		m_mat[2][3] = z3;
		m_mat[3][0] = w0;
		m_mat[3][1] = w1;
		m_mat[3][2] = w2;
		m_mat[3][3] = w3;
	}

	Matrix4x4 Matrix4x4::add(const Matrix4x4& a, const Matrix4x4& b)
	{
		Matrix4x4 result;
		result.m_mat[0][0] = a.m_mat[0][0] + b.m_mat[0][0];
		result.m_mat[0][1] = a.m_mat[0][1] + b.m_mat[0][1];
		result.m_mat[0][2] = a.m_mat[0][2] + b.m_mat[0][2];
		result.m_mat[0][3] = a.m_mat[0][3] + b.m_mat[0][3];

		result.m_mat[1][0] = a.m_mat[1][0] + b.m_mat[1][0];
		result.m_mat[1][1] = a.m_mat[1][1] + b.m_mat[1][1];
		result.m_mat[1][2] = a.m_mat[1][2] + b.m_mat[1][2];
		result.m_mat[1][3] = a.m_mat[1][3] + b.m_mat[1][3];

		result.m_mat[2][0] = a.m_mat[2][0] + b.m_mat[2][0];
		result.m_mat[2][1] = a.m_mat[2][1] + b.m_mat[2][1];
		result.m_mat[2][2] = a.m_mat[2][2] + b.m_mat[2][2];
		result.m_mat[2][3] = a.m_mat[2][3] + b.m_mat[2][3];

		result.m_mat[3][0] = a.m_mat[3][0] + b.m_mat[3][0];
		result.m_mat[3][1] = a.m_mat[3][1] + b.m_mat[3][1];
		result.m_mat[3][2] = a.m_mat[3][2] + b.m_mat[3][2];
		result.m_mat[3][3] = a.m_mat[3][3] + b.m_mat[3][3];

		return result;
	}

	Matrix4x4 Matrix4x4::subtract(const Matrix4x4& a, const Matrix4x4& b)
	{
		Matrix4x4 result;
		result.m_mat[0][0] = a.m_mat[0][0] - b.m_mat[0][0];
		result.m_mat[0][1] = a.m_mat[0][1] - b.m_mat[0][1];
		result.m_mat[0][2] = a.m_mat[0][2] - b.m_mat[0][2];
		result.m_mat[0][3] = a.m_mat[0][3] - b.m_mat[0][3];

		result.m_mat[1][0] = a.m_mat[1][0] - b.m_mat[1][0];
		result.m_mat[1][1] = a.m_mat[1][1] - b.m_mat[1][1];
		result.m_mat[1][2] = a.m_mat[1][2] - b.m_mat[1][2];
		result.m_mat[1][3] = a.m_mat[1][3] - b.m_mat[1][3];

		result.m_mat[2][0] = a.m_mat[2][0] - b.m_mat[2][0];
		result.m_mat[2][1] = a.m_mat[2][1] - b.m_mat[2][1];
		result.m_mat[2][2] = a.m_mat[2][2] - b.m_mat[2][2];
		result.m_mat[2][3] = a.m_mat[2][3] - b.m_mat[2][3];

		result.m_mat[3][0] = a.m_mat[3][0] - b.m_mat[3][0];
		result.m_mat[3][1] = a.m_mat[3][1] - b.m_mat[3][1];
		result.m_mat[3][2] = a.m_mat[3][2] - b.m_mat[3][2];
		result.m_mat[3][3] = a.m_mat[3][3] - b.m_mat[3][3];

		return result;
	}

	Matrix4x4 Matrix4x4::multiply(const Matrix4x4& a, const Matrix4x4& b)
	{
		Matrix4x4 result;

		result.m_mat[0][0] = a.m_mat[0][0] * b.m_mat[0][0] + a.m_mat[0][1] * b.m_mat[1][0] + a.m_mat[0][2] * b.m_mat[2][0] + a.m_mat[0][3] * b.m_mat[3][0];
		result.m_mat[0][1] = a.m_mat[0][0] * b.m_mat[0][1] + a.m_mat[0][1] * b.m_mat[1][1] + a.m_mat[0][2] * b.m_mat[2][1] + a.m_mat[0][3] * b.m_mat[3][1];
		result.m_mat[0][2] = a.m_mat[0][0] * b.m_mat[0][2] + a.m_mat[0][1] * b.m_mat[1][2] + a.m_mat[0][2] * b.m_mat[2][2] + a.m_mat[0][3] * b.m_mat[3][2];
		result.m_mat[0][3] = a.m_mat[0][0] * b.m_mat[0][3] + a.m_mat[0][1] * b.m_mat[1][3] + a.m_mat[0][2] * b.m_mat[2][3] + a.m_mat[0][3] * b.m_mat[3][3];

		result.m_mat[1][0] = a.m_mat[1][0] * b.m_mat[0][0] + a.m_mat[1][1] * b.m_mat[1][0] + a.m_mat[1][2] * b.m_mat[2][0] + a.m_mat[1][3] * b.m_mat[3][0];
		result.m_mat[1][1] = a.m_mat[1][0] * b.m_mat[0][1] + a.m_mat[1][1] * b.m_mat[1][1] + a.m_mat[1][2] * b.m_mat[2][1] + a.m_mat[1][3] * b.m_mat[3][1];
		result.m_mat[1][2] = a.m_mat[1][0] * b.m_mat[0][2] + a.m_mat[1][1] * b.m_mat[1][2] + a.m_mat[1][2] * b.m_mat[2][2] + a.m_mat[1][3] * b.m_mat[3][2];
		result.m_mat[1][3] = a.m_mat[1][0] * b.m_mat[0][3] + a.m_mat[1][1] * b.m_mat[1][3] + a.m_mat[1][2] * b.m_mat[2][3] + a.m_mat[1][3] * b.m_mat[3][3];

		result.m_mat[2][0] = a.m_mat[2][0] * b.m_mat[0][0] + a.m_mat[2][1] * b.m_mat[1][0] + a.m_mat[2][2] * b.m_mat[2][0] + a.m_mat[2][3] * b.m_mat[3][0];
		result.m_mat[2][1] = a.m_mat[2][0] * b.m_mat[0][1] + a.m_mat[2][1] * b.m_mat[1][1] + a.m_mat[2][2] * b.m_mat[2][1] + a.m_mat[2][3] * b.m_mat[3][1];
		result.m_mat[2][2] = a.m_mat[2][0] * b.m_mat[0][2] + a.m_mat[2][1] * b.m_mat[1][2] + a.m_mat[2][2] * b.m_mat[2][2] + a.m_mat[2][3] * b.m_mat[3][2];
		result.m_mat[3][2] = a.m_mat[2][0] * b.m_mat[0][3] + a.m_mat[2][1] * b.m_mat[1][3] + a.m_mat[2][2] * b.m_mat[2][3] + a.m_mat[2][3] * b.m_mat[3][3];

		result.m_mat[3][0] = a.m_mat[3][0] * b.m_mat[0][0] + a.m_mat[3][1] * b.m_mat[1][0] + a.m_mat[3][2] * b.m_mat[2][0] + a.m_mat[3][3] * b.m_mat[3][0];
		result.m_mat[3][1] = a.m_mat[3][0] * b.m_mat[0][1] + a.m_mat[3][1] * b.m_mat[1][1] + a.m_mat[3][2] * b.m_mat[2][1] + a.m_mat[3][3] * b.m_mat[3][1];
		result.m_mat[3][2] = a.m_mat[3][0] * b.m_mat[0][2] + a.m_mat[3][1] * b.m_mat[1][2] + a.m_mat[3][2] * b.m_mat[2][2] + a.m_mat[3][3] * b.m_mat[3][2];
		result.m_mat[3][3] = a.m_mat[3][0] * b.m_mat[0][3] + a.m_mat[3][1] * b.m_mat[1][3] + a.m_mat[3][2] * b.m_mat[2][3] + a.m_mat[3][3] * b.m_mat[3][3];

		return result;
	}

	Matrix4x4 Matrix4x4::operator-()
	{
		return Matrix4x4
				(
						m_mat[0][0] * -1, m_mat[0][1] * -1, m_mat[0][2] * -1, m_mat[0][3] * -1,
						m_mat[1][0] * -1, m_mat[1][1] * -1, m_mat[1][2] * -1, m_mat[1][3] * -1,
						m_mat[2][0] * -1, m_mat[2][1] * -1, m_mat[2][2] * -1, m_mat[2][3] * -1,
						m_mat[3][0] * -1, m_mat[3][1] * -1, m_mat[3][2] * -1, m_mat[3][3] * -1
				);
	}

	Matrix4x4 Matrix4x4::operator+(const Matrix4x4& a)
	{
		return add(*this, a);
	}

	Matrix4x4 Matrix4x4::operator-(const Matrix4x4& a)
	{
		return subtract(*this, a);
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& a)
	{
		return multiply(*this, a);
	}

	std::ostream& operator<<(std::ostream& out, Matrix4x4& mat)
	{
		out << "{" << mat.m_mat[0][0] << ", " << mat.m_mat[0][1] << ", " << mat.m_mat[0][2] << ", " << mat.m_mat[0][3] << "}" << '\n';
		out << "{" << mat.m_mat[1][0] << ", " << mat.m_mat[1][1] << ", " << mat.m_mat[1][2] << ", " << mat.m_mat[1][3] << "}" << '\n';
		out << "{" << mat.m_mat[2][0] << ", " << mat.m_mat[2][1] << ", " << mat.m_mat[2][2] << ", " << mat.m_mat[2][3] << "}" << '\n';
		out << "{" << mat.m_mat[3][0] << ", " << mat.m_mat[3][1] << ", " << mat.m_mat[3][2] << ", " << mat.m_mat[3][3] << "}";

		return out;
	}

	Matrix4x4 identity4()
	{
		return Matrix4x4(1.0f);
	}
}