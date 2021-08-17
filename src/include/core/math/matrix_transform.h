#pragma once

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "matrix3x3.h"
#include "matrix4x4.h"

namespace halogen::math
{
	//Goal is to take a matrix as input (mostly a identity matrix) and provide a transformation matrix that
	//can be multiplied with a vector to transform the vector. Similar to GLM in this regard.

	inline Matrix3x3 scale_matrix(Matrix3x3& mat, const Vector3& vec)
	{
		Matrix3x3 result = mat;

		result.m_mat[0][0] = vec.m_x;
		result.m_mat[1][1] = vec.m_y;
		result.m_mat[2][2] = vec.m_z;

		return result;
	}

	inline Matrix4x4 scale_matrix(Matrix4x4& mat, const Vector4& vec)
	{
		Matrix4x4 result = mat;

		result.m_mat[0][0] = vec.m_x;
		result.m_mat[1][1] = vec.m_y;
		result.m_mat[2][2] = vec.m_z;

		return result;
	}

	inline Matrix3x3 translate_matrix(Matrix3x3& mat, const Vector3& vec)
	{
		Matrix3x3 result = mat;

		result.m_mat[0][2] = vec.m_x;
		result.m_mat[1][2] = vec.m_y;
		result.m_mat[2][2] = vec.m_z;

		return result;
	}

	inline Matrix4x4 translate_matrix(Matrix4x4& mat, const Vector4& vec)
	{
		Matrix4x4 result = mat;

		result.m_mat[0][2] = vec.m_x;
		result.m_mat[1][2] = vec.m_y;
		result.m_mat[2][2] = vec.m_z;

		return result;
	}

	inline Matrix3x3 rotate_matrix_x(Matrix3x3& mat, float radians)
	{
		Matrix3x3 result;

		result.m_mat[0][0] = 1;
		result.m_mat[0][1] = 0;
		result.m_mat[0][2] = 0;

		result.m_mat[1][0] = 0;
		result.m_mat[1][1] = cosf(radians);
		result.m_mat[1][2] = -sinf(radians);

		result.m_mat[2][0] = 0;
		result.m_mat[2][1] = sinf(radians);
		result.m_mat[2][2] = cosf(radians);

		return result;
	}

	inline Matrix3x3 rotate_matrix_y(Matrix3x3& mat, float radians)
	{
		Matrix3x3 result;

		result.m_mat[0][0] = cosf(radians);
		result.m_mat[0][1] = 0;
		result.m_mat[0][2] = sinf(radians);

		result.m_mat[1][0] = 0;
		result.m_mat[1][1] = 1;
		result.m_mat[1][2] = 0;

		result.m_mat[2][0] = -sinf(radians);
		result.m_mat[2][1] = 0;
		result.m_mat[2][2] = cosf(radians);

		return result;
	}

	inline Matrix3x3 rotate_matrix_z(Matrix3x3& mat, float radians)
	{
		Matrix3x3 result;

		result.m_mat[0][0] = cosf(radians);
		result.m_mat[0][1] = -sinf(radians);
		result.m_mat[0][2] = 0;

		result.m_mat[1][0] = sinf(radians);
		result.m_mat[1][1] = cosf(radians);
		result.m_mat[1][2] = 0;

		result.m_mat[2][0] = 0;
		result.m_mat[2][1] = 0;
		result.m_mat[2][2] = 1;

		return result;
	}

	inline Matrix4x4 rotate_matrix_x(Matrix4x4& mat, float radians)
	{
		Matrix4x4 result;

		result.m_mat[0][0] = 1;
		result.m_mat[0][1] = 0;
		result.m_mat[0][2] = 0;
		result.m_mat[0][3] = 0;

		result.m_mat[1][0] = 0;
		result.m_mat[1][1] = cosf(radians);
		result.m_mat[1][2] = -sinf(radians);
		result.m_mat[1][3] = 0;

		result.m_mat[2][0] = 0;
		result.m_mat[2][1] = sinf(radians);
		result.m_mat[2][2] = cosf(radians);
		result.m_mat[2][3] = 0;

		result.m_mat[3][0] = 0;
		result.m_mat[3][1] = 0;
		result.m_mat[3][2] = 0;
		result.m_mat[3][3] = 1;

		return result;
	}

	inline Matrix4x4 rotate_matrix_y(Matrix4x4& mat, float radians)
	{
		Matrix4x4 result;

		result.m_mat[0][0] = cosf(radians);
		result.m_mat[0][1] = 0;
		result.m_mat[0][2] = sinf(radians);
		result.m_mat[0][3] = 0;

		result.m_mat[1][0] = 0;
		result.m_mat[1][1] = 1;
		result.m_mat[1][2] = 0;
		result.m_mat[1][3] = 0;

		result.m_mat[2][0] = -sinf(radians);
		result.m_mat[2][1] = 0;
		result.m_mat[2][2] = cosf(radians);
		result.m_mat[2][3] = 0;

		result.m_mat[3][0] = 0;
		result.m_mat[3][1] = 0;
		result.m_mat[3][2] = 0;
		result.m_mat[3][3] = 1;

		return result;
	}

	inline Matrix4x4 rotate_matrix_z(Matrix4x4& mat, float radians)
	{
		Matrix4x4 result;

		result.m_mat[0][0] = cosf(radians);
		result.m_mat[0][1] = -sinf(radians);
		result.m_mat[0][2] = 0;
		result.m_mat[0][3] = 0;

		result.m_mat[1][0] = sinf(radians);
		result.m_mat[1][1] = cosf(radians);
		result.m_mat[1][2] = 0;
		result.m_mat[1][3] = 0;

		result.m_mat[2][0] = 0;
		result.m_mat[2][1] = 0;
		result.m_mat[2][2] = 1;
		result.m_mat[2][3] = 0;

		result.m_mat[3][0] = 0;
		result.m_mat[3][1] = 0;
		result.m_mat[3][2] = 0;
		result.m_mat[3][3] = 1;

		return result;
	}
}