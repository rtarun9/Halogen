#pragma once

#include <cmath>
#include <algorithm>
#include <initializer_list>

#define CML_FUNC static inline

CML_FUNC size_t min(const size_t& a, const size_t& b)
{
	return a < b ? a : b;
}

// specializations
template <int n>
struct Vector
{
	Vector(): x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{
		for (int i = 0; i < n; i++)
		{
			data[i] = 0.0f;
		}
	}
	
	Vector(std::initializer_list<float> init_list)
	{
		size_t vec_size = min(n, init_list.size());
		auto iter = init_list.begin();

		for (size_t i = 0; i < vec_size; i++)
		{
			data[i] = *iter;
			++iter;
		}

		for (size_t i = vec_size; i < n; i++)
		{
			data[i] = 0.0f;
		}
	}

	float& operator[](size_t index)
	{
		return data[index];
	}

	const float& operator[](size_t index) const
	{
		return data[index];
	}

	friend Vector operator+(const Vector& a, const Vector& b)
	{
		Vector res{};

		for (int i = 0; i < n; i++)
		{
			res[i] = a[i] + b[i];
		}

		return res;
	}

	friend Vector operator-(const Vector& a, const Vector& b)
	{
		Vector res{};

		for (int i = 0; i < n; i++)
		{
			res[i] = a[i] - b[i];
		}

		return res;
	}

	friend Vector operator*(const Vector& a, const Vector& b)
	{
		Vector res{};

		for (int i = 0; i < n; i++)
		{
			res[i] = a[i] * b[i];
		}

		return res;
	}

	friend Vector operator/(const Vector& a, const Vector& b)
	{
		Vector res{};

		for (int i = 0; i < n; i++)
		{
			res[i] = a[i] / b[i];
		}

		return res;
	}
	
	Vector& operator+=(const Vector& other)
	{
		for (int i = 0; i < n; i++)
		{
			this->data[i] += other[i];
		}

		return *this;
	}

	Vector& operator-=(const Vector& other)
	{
		for (int i = 0; i < n; i++)
		{
			this->data[i] -= other[i];
		}

		return *this;
	}

	Vector& operator*=(const Vector& other)
	{
		for (int i = 0; i < n; i++)
		{
			this->data[i] *= other[i];
		}

		return *this;
	}

	Vector& operator/=(const Vector& other)
	{
		for (int i = 0; i < n; i++)
		{
			this->data[i] /= other[i];
		}

		return *this;
	}

	friend float dot(const Vector& a, const Vector& b)
	{
		float res = 0;
		for (int i = 0; i < n; i++)
		{
			res += a[i] * b[i];
		}

		return res;
	}

	union
	{
		float data[n];
		struct
		{
			float x, y, z, w;
		};
	};
};

using Vec2 = Vector<2>;
using Vec3 = Vector<3>;
using Vec4 = Vector<4>;

template<int row, int column>
struct Matrix
{
	Matrix(std::initializer_list<float> init_list)
	{
		size_t m = min(row * column, init_list.size());

		auto iter = init_list.begin();

		for (size_t i = 0; i < m; i++)
		{
			data[i]  = *iter;
			++iter;
		}

		for (size_t i = m; i < row * column; i++)
		{
			data[i] = {0.0f};
		}
	}

	Matrix(std::initializer_list<Vector<row>> init_list)
	{
		size_t m = min(row * column, init_list.size());

		auto iter = init_list.begin();

		for (size_t i = 0; i < m; i++)
		{
			data[i] = *iter;
			++iter;
		}

		for (size_t i = m; i < row * column; i++)
		{
			data[i] = { 0.0f };
		}
	}

	friend Matrix operator+(const Matrix& a, const Matrix& b)
	{
		Matrix res{};

		for (int i = 0; i < row * column; i++)
		{
			res[i] = a[i] + b[i];
		}

		return res;
	}

	friend Matrix operator-(const Matrix& a, const Matrix& b)
	{
		Matrix res{};

		for (int i = 0; i < row * column; i++)
		{
			res[i] = a[i] - b[i];
		}

		return res;
	}

	friend Matrix operator/(const Matrix& a, const Matrix& b)
	{
		Matrix res{};

		for (int i = 0; i < row * column; i++)
		{
			res[i] = a[i] / b[i];
		}

		return res;
	}

	friend Matrix operator*(const Matrix& a, const Matrix& b)
	{
		Matrix res{};

		for (int i = 0; i < row * column; i++)
		{
			res[i] = a[i] * b[i];
		}

		return res;
	}

	friend Matrix scale(const Vector<row>& vec)
	{
		Matrix res{};
		for (int i = 0; i < row; i++)
		{
			res.data[i][i] = vec[i];
		}

		return res;
	}

	friend Matrix translate(const Vector<row>& vec)
	{
		Matrix res{};
		for (int j = 0; i < row; i++)
		{
			res[column][j] = vec[j];
		}

		return res;
	}

	friend Vector<row> operator*(const Matrix& a, const Vector<row>& vec)
	{
		Vector<row> res{};

		for (int i = 0; i < row; i++)
		{
			float col_sum = 0;
			for (int j = 0; j < column; j++)
			{
				col_sum += a.data_rc[i][j];
			}

			res[i] = vec[i] * col_sum;
		}

		return res;
	}

	Matrix& operator-=(const Matrix& other)
	{
		for (int i = 0; i < row * column; i++)
		{
			this->data[i] -= other[i];
		}

		return *this;
	}

	Matrix& operator+=(const Matrix& other)
	{
		for (int i = 0; i < row * column; i++)
		{
			this->data[i] += other[i];
		}

		return *this;
	}

	Matrix& operator*=(const Matrix& other)
	{
		for (int i = 0; i < row * column; i++)
		{
			this->data[i] *= other[i];
		}

		return *this;
	}

	Matrix& operator/=(const Matrix& other)
	{
		for (int i = 0; i < row * column; i++)
		{
			this->data[i] /= other[i];
		}

		return *this;
	}

	union
	{
		float data_rc[row][column];
		struct
		{
			float rows[row];
		};
		struct
		{
			float data[row * column];
		};
	};
};

using Mat3 = Matrix<3, 3>;
using Mat4 = Matrix<4, 4>;


