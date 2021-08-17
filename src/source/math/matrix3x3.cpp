#include "../../include/core/math/matrix3x3.h"

namespace halogen::math
{

	Matrix3x3::Matrix3x3()
	{
		m_mat[0][2] = 0.0f;
		m_mat[0][0] = 0.0f;
		m_mat[1][0] = 0.0f;
		m_mat[1][1] = 0.0f;
		m_mat[1][2] = 0.0f;
		m_mat[2][0] = 0.0f;
		m_mat[2][1] = 0.0f;
		m_mat[0][1] = 0.0f;
		m_mat[2][2] = 0.0f;
	}

	Matrix3x3::Matrix3x3(float value)
	{
		m_mat[0][0] = value;
		m_mat[0][1] = 0.0f;
		m_mat[0][2] = 0.0f;
		m_mat[1][0] = 0.0f;
		m_mat[1][1] = value;
		m_mat[1][2] = 0.0f;
		m_mat[2][0] = 0.0f;
		m_mat[2][1] = 0.0f;
		m_mat[2][2] = value;
	}

	Matrix3x3::Matrix3x3(float x, float y, float z)
	{
		m_mat[0][0] = (x);
		m_mat[0][1] = 0.0f;
		m_mat[0][2] = 0.0f;
		m_mat[1][0] = 0.0f;
		m_mat[1][1] = (y);
		m_mat[1][2] = 0.0f;
		m_mat[2][0] = 0.0f;
		m_mat[2][1] = 0.0f;
		m_mat[2][2] = (z);
	}

	Matrix3x3::Matrix3x3(float x0, float x1, float x2, float y0, float y1, float y2, float z0, float z1, float z2)
	{
		m_mat[0][0] = (x0);
		m_mat[0][1] = (x1);
		m_mat[0][2] = (x2);
		m_mat[1][0] = (y0);
		m_mat[1][1] = (y1);
		m_mat[1][2] = (y2);
		m_mat[2][0] = (z0);
		m_mat[2][1] = (z1);
		m_mat[2][2] = (z2);
	}

	Matrix3x3 Matrix3x3::add(const Matrix3x3& a, const Matrix3x3& b)
	{
		Matrix3x3 result;
		result.m_mat[0][0] = a.m_mat[0][0] + b.m_mat[0][0];
		result.m_mat[0][1] = a.m_mat[0][1] + b.m_mat[0][1];
		result.m_mat[0][2] = a.m_mat[0][2] + b.m_mat[0][2];

		result.m_mat[1][0] = a.m_mat[1][0] + b.m_mat[1][0];
		result.m_mat[1][1] = a.m_mat[1][1] + b.m_mat[1][1];
		result.m_mat[1][2] = a.m_mat[1][2] + b.m_mat[1][2];

		result.m_mat[2][0] = a.m_mat[2][0] + b.m_mat[2][0];
		result.m_mat[2][1] = a.m_mat[2][1] + b.m_mat[2][1];
		result.m_mat[2][2] = a.m_mat[2][2] + b.m_mat[2][2];

		return result;
	}

	Matrix3x3 Matrix3x3::subtract(const Matrix3x3& a, const Matrix3x3& b)
	{
		Matrix3x3 result;
		result.m_mat[0][0] = a.m_mat[0][0] - b.m_mat[0][0];
		result.m_mat[0][1] = a.m_mat[0][1] - b.m_mat[0][1];
		result.m_mat[0][2] = a.m_mat[0][2] - b.m_mat[0][2];

		result.m_mat[1][0] = a.m_mat[1][0] - b.m_mat[1][0];
		result.m_mat[1][1] = a.m_mat[1][1] - b.m_mat[1][1];
		result.m_mat[1][2] = a.m_mat[1][2] - b.m_mat[1][2];

		result.m_mat[2][0] = a.m_mat[2][0] - b.m_mat[2][0];
		result.m_mat[2][1] = a.m_mat[2][1] - b.m_mat[2][1];
		result.m_mat[2][2] = a.m_mat[2][2] - b.m_mat[2][2];

		return result;
	}

	Matrix3x3 Matrix3x3::multiply(const Matrix3x3& a, const Matrix3x3& b)
	{
		Matrix3x3 result;
		result.m_mat[0][0] = a.m_mat[0][0] * b.m_mat[0][0] + a.m_mat[0][1] * b.m_mat[1][0] + a.m_mat[0][2] * b.m_mat[2][0];
		result.m_mat[0][1] = a.m_mat[0][0] * b.m_mat[0][1] + a.m_mat[0][1] * b.m_mat[1][1] + a.m_mat[0][2] * b.m_mat[2][1];
		result.m_mat[0][2] = a.m_mat[0][0] * b.m_mat[0][2] + a.m_mat[0][1] * b.m_mat[1][2] + a.m_mat[0][2] * b.m_mat[2][2];

		result.m_mat[1][0] = a.m_mat[1][0] * b.m_mat[0][0] + a.m_mat[1][1] * b.m_mat[1][0] + a.m_mat[1][2] * b.m_mat[2][0];
		result.m_mat[1][1] = a.m_mat[1][0] * b.m_mat[0][1] + a.m_mat[1][1] * b.m_mat[1][1] + a.m_mat[1][2] * b.m_mat[2][1];
		result.m_mat[1][2] = a.m_mat[1][0] * b.m_mat[0][2] + a.m_mat[1][1] * b.m_mat[1][2] + a.m_mat[1][2] * b.m_mat[2][2];

		result.m_mat[2][0] = a.m_mat[2][0] * b.m_mat[0][0] + a.m_mat[2][1] * b.m_mat[1][0] + a.m_mat[2][2] * b.m_mat[2][0];
		result.m_mat[2][1] = a.m_mat[2][0] * b.m_mat[0][1] + a.m_mat[2][1] * b.m_mat[1][1] + a.m_mat[2][2] * b.m_mat[2][1];
		result.m_mat[2][2] = a.m_mat[2][0] * b.m_mat[0][2] + a.m_mat[2][1] * b.m_mat[1][2] + a.m_mat[2][2] * b.m_mat[2][2];

		return result;
	}

	Matrix3x3 Matrix3x3::operator-()
	{
		return Matrix3x3
		(
			m_mat[0][0] * -1, m_mat[0][1] * -1, m_mat[0][2] * -1,
			m_mat[1][0] * -1, m_mat[1][1] * -1, m_mat[1][2] * -1,
			m_mat[2][0] * -1, m_mat[2][1] * -1, m_mat[2][2] * -1
		);
	}

	Matrix3x3 Matrix3x3::operator+(const Matrix3x3& a)
	{
		return add(*this, a);
	}

	Matrix3x3 Matrix3x3::operator-(const Matrix3x3& a)
	{
		return subtract(*this, a);
	}

	Matrix3x3 Matrix3x3::operator*(const Matrix3x3& a)
	{
		return multiply(*this, a);
	}

	std::ostream& operator<<(std::ostream& out, Matrix3x3& mat)
	{
		out << "{" << mat.m_mat[0][0] << ", " << mat.m_mat[0][1] << ", " << mat.m_mat[0][2] << "}" << '\n';
		out << "{" << mat.m_mat[1][0] << ", " << mat.m_mat[1][1] << ", " << mat.m_mat[1][2] << "}" << '\n';
		out << "{" << mat.m_mat[2][0] << ", " << mat.m_mat[2][1] << ", " << mat.m_mat[2][2] << "}";

		return out;
	}

	Matrix3x3 identity3()
	{
		return Matrix3x3(1.0f);
	}
}