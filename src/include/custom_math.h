#pragma once

#include <cmath>
#include <algorithm>
#include <initializer_list>

// note : this math lib is in no way "a perfect replacement to glm". It has many issues, but made for learning purposes.
// note : row major order is used here. all vector & matrix functions are in 2 generic templated classes.
// note : look at the bottom of the file for the specializations, which are done using 'using V4 = Vector<4>'. Only dimension is templated, not the data type
#define CML_FUNC static inline
#define CML_NO_DISCARD [[nodiscard]]

CML_NO_DISCARD
CML_FUNC size_t min(const size_t& a, const size_t& b)
{
	return a < b ? a : b;
}

CML_NO_DISCARD
CML_FUNC float radians(float f)
{
	return f * 3.14159f / 180.0f;
}

namespace halo::math
{ 
	// specialization for vector class
	template <int N>
	struct Vector
	{
		Vector() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
		{
			for (int i = 0; i < N; i++)
			{
				data[i] = 0.0f;
			}
		}
	
		Vector(std::initializer_list<float> init_list)
		{
			size_t vec_size = min(N, init_list.size());
			auto iter = init_list.begin();
	
			for (size_t i = 0; i < vec_size; i++)
			{
				data[i] = *iter;
				++iter;
			}
	
			for (size_t i = vec_size; i < N; i++)
			{
				data[i] = 0.0f;
			}
		}
	
		CML_NO_DISCARD
		float& operator[](size_t index)
		{
			return data[index];
		}
	
		CML_NO_DISCARD
		const float& operator[](size_t index) const
		{
			return data[index];
		}
	    
		CML_NO_DISCARD
		friend Vector operator+(const Vector& a, const Vector& b)
		{
			Vector res{};
	
			for (int i = 0; i < N; i++)
			{
				res[i] = a[i] + b[i];
			}
	
			return res;
		}
		
		CML_NO_DISCARD
		friend Vector operator-(const Vector& a, const Vector& b)
		{
			Vector res{};
	
			for (int i = 0; i < N; i++)
			{
				res[i] = a[i] - b[i];
			}
	
			return res;
		}
	
		CML_NO_DISCARD
		friend Vector operator*(const Vector& a, const Vector& b)
		{
			Vector res{};
	
			for (int i = 0; i < N; i++)
			{
				res[i] = a[i] * b[i];
			}
	
			return res;
		}
	
		CML_NO_DISCARD
		friend Vector operator/(const Vector& a, const Vector& b)
		{
			Vector res{};
	
			for (int i = 0; i < N; i++)
			{
				res[i] = a[i] / b[i];
			}
	
			return res;
		}

		Vector& operator+=(const Vector& other)
		{
			for (int i = 0; i < N; i++)
			{
				this->data[i] += other[i];
			}
	
			return *this;
		}
	
		Vector& operator-=(const Vector& other)
		{
			for (int i = 0; i < N; i++)
			{
				this->data[i] -= other[i];
			}
	
			return *this;
		}
	
		Vector& operator*=(const Vector& other)
		{
			for (int i = 0; i < N; i++)
			{
				this->data[i] *= other[i];
			}
	
			return *this;
		}
	
		Vector& operator/=(const Vector& other)
		{
			for (int i = 0; i < N; i++)
			{
				this->data[i] /= other[i];
			}
	
			return *this;
		}
			
		CML_NO_DISCARD
		Vector& operator*(const float f)
		{
			for (int i = 0; i < N; i++)
			{
				this->data[i] *= f;
			}

			return *this;
		}

		CML_NO_DISCARD
		const float length() const
		{
			float len = 0;
			for (int i = 0; i < N; i++)
			{
				len += data[i] * data[i];
			}

			return sqrtf(len);
		}

		CML_NO_DISCARD
		Vector& normalize()
		{
			for (int i = 0; i < N; i++)
			{
				this->data[i] /= length();
			}

			return *this;
		}

		CML_NO_DISCARD
		friend Vector& normalize(const Vector& vec)
		{
			return vec / vec.length();
		}

		CML_NO_DISCARD
		friend float dot(const Vector& a, const Vector& b)
		{
			float res = 0;
			for (int i = 0; i < N; i++)
			{
				res += a[i] * b[i];
			}
	
			return res;
		}
	
		union
		{
			float data[N];
			struct
			{
				float x, y, z, w;
			};

			struct
			{
				float r, g, b, a;
			};
		};
	};
	
	// special funcs for Vector<3>
	CML_NO_DISCARD
	CML_FUNC Vector<3> cross(const Vector<3>& a, const Vector<3>& b)
	{
		return Vector<3>{a.y * b.z - b.y * a.z, -(a.x * b.z - a.z * b.x), a.x * b.y - a.y * b.x};
	}

	template <int Row, int Column>
	struct Matrix
	{
		Matrix()
		{
			for (int i = 0; i < Row * Column; i++)
			{
				data[i] = 0.0f;
			}
		}

		Matrix(float v)
		{
			for (int i = 0; i < Row; i++)
			{
				for (int j = 0; j < Column; j++)
				{
					data_rc[i][j] = i == j ? v : 0.0f;
				}
			}
		}

		Matrix(std::initializer_list<float> init_list)
		{
			size_t m = min(Row * Column, init_list.size());
	
			auto iter = init_list.begin();
	
			for (size_t i = 0; i < m; i++)
			{
				data[i] = *iter;
				++iter;
			}
	
			for (size_t i = m; i < Row * Column; i++)
			{
				data[i] = { 0.0f };
			}
		}
	
		Matrix(std::initializer_list<Vector<Row>> init_list)
		{
			size_t m = min(Row * Column, init_list.size());
	
			auto iter = init_list.begin();
	
			for (size_t i = 0; i < m; i++)
			{
				data[i] = *iter;
				++iter;
			}
	
			for (size_t i = m; i < Row * Column; i++)
			{
				data[i] = { 0.0f };
			}
		}
	
		CML_NO_DISCARD
		float& operator[](int index)
		{
			return data[index];
		}

		CML_NO_DISCARD
		friend Matrix operator+(const Matrix& a, const Matrix& b)
		{
			Matrix res{};
	
			for (int i = 0; i < Row * Column; i++)
			{
				res[i] = a[i] + b[i];
			}
	
			return res;
		}
	
		CML_NO_DISCARD
		friend Matrix operator-(const Matrix& a, const Matrix& b)
		{
			Matrix res{};
	
			for (int i = 0; i < Row * Column; i++)
			{
				res[i] = a[i] - b[i];
			}
	
			return res;
		}
	
		CML_NO_DISCARD
		friend Matrix operator/(const Matrix& a, const Matrix& b)
		{
			Matrix res{};
	
			for (int i = 0; i < Row * Column; i++)
			{
				res[i] = a[i] / b[i];
			}
	
			return res;
		}
	
		CML_NO_DISCARD
		friend Matrix operator*(const Matrix& a, const Matrix& b)
		{
			Matrix res{};
	
			for (int i = 0; i < Row; i++)
			{
				for (int j = 0; j < Column; j++)
				{
					for (int k = 0; k < Column; k++)
					{
						res.data_rc[i][j] += a.data_rc[i][k] * b.data_rc[k][j];
					}
				}
			}
	
			return res;
		}
	 
		CML_NO_DISCARD
		friend Vector<Row> operator*(const Matrix& a, const Vector<Row>& vec)
		{
			Vector<Row> res{};
	
			for (int i = 0; i < Row; i++)
			{
				float col_sum = 0;
				for (int j = 0; j < Column; j++)
				{
					col_sum += a.data_rc[i][j];
				}
	
				res[i] = vec[i] * col_sum;
			}
	
			return res;
		}
	
		CML_NO_DISCARD
		Matrix& operator-=(const Matrix& other)
		{
			for (int i = 0; i < Row * Column; i++)
			{
				this->data[i] -= other[i];
			}
	
			return *this;
		}
	
		CML_NO_DISCARD
		Matrix& operator+=(const Matrix& other)
		{
			for (int i = 0; i < Row * Column; i++)
			{
				this->data[i] += other[i];
			}
	
			return *this;
		}
	
		CML_NO_DISCARD
		Matrix& operator*=(const Matrix& other)
		{
			for (int i = 0; i < Row * Column; i++)
			{
				this->data[i] *= other[i];
			}
	
			return *this;
		}
	
		CML_NO_DISCARD
		Matrix& operator/=(const Matrix& other)
		{
			for (int i = 0; i < Row * Column; i++)
			{
				this->data[i] /= other[i];
			}
	
			return *this;
		}
	
		union
		{
			float data_rc[Row][Column];
			struct
			{
				float Rows[Row];
			};
			struct
			{
				float data[Row * Column];
			};
		};
	};
	
	// transformations
	
	CML_NO_DISCARD
	CML_FUNC Matrix<4, 4> translate(const Vector<3>& vec)
	{
		Matrix<4, 4> res = Matrix<4, 4>(1.0f);

		res.data_rc[0][3] = vec[0];
		res.data_rc[1][3] = vec[1];
		res.data_rc[2][3] = vec[2];

		return res;
	}

	CML_NO_DISCARD
	CML_FUNC Matrix<4, 4> scale(const Vector<3>& vec)
	{
		return Matrix<4, 4>
		{
			1.0f, 0.0f, 0.0f, vec[0],
				0.0f, 1.0f, 0.0f, vec[1],
				0.0f, 0.0f, 1.0f, vec[2],
				0.0f, 0.0f, 0.0f, 1.0f
		};
	}
	
	CML_NO_DISCARD
	CML_FUNC Matrix<4, 4> rotate_x(float theta)
	{
		float cos_x = cos(radians(theta));
		float sin_x = sin(radians(theta));
	
		return Matrix<4, 4>
		{
			1, 0, 0, 0,
				0, cos_x, sin_x, 0,
				0, -sin_x, cos_x, 0,
				0, 0, 0, 1
		};
	}
	
	CML_NO_DISCARD
	CML_FUNC Matrix<4, 4> rotate_y(float theta)
	{
		float cos_y = cos(radians(theta));
		float sin_y = sin(radians(theta));
	
		return Matrix<4, 4>
		{
			cos_y, 0.0f, -sin_y, 0,
				0.0f, 1.0f, 0.0f, 0,
				sin_y, 0.0f, cos_y, 0,
				0, 0, 0, 1
		};
	}
	
	CML_NO_DISCARD
	CML_FUNC Matrix<4, 4> rotate_z(float theta)
	{
		float cos_z = cos(radians(theta));
		float sin_z = sin(radians(theta));
	
		return Matrix<4, 4>
		{
			cos_z, sin_z, 0, 0,
				-sin_z, cos_z, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1
		};
	}
	
	CML_NO_DISCARD
	CML_FUNC Matrix<4, 4> perpective(float fov, float aspect_ratio, float near, float far)
	{
		return Matrix<4, 4>
		{
			1.0f / (aspect_ratio * tanf(fov / 2)), 0, 0, 0,
				0, 1.0f / tanf(fov / 2), 0, 0,
				0, 0, far / (far - near), -far * near / (far - near),
				0, 0, 1, 0
		};
	}
	
	CML_NO_DISCARD
	CML_FUNC Matrix<4, 4>  look_at(const Vector<3>& right, const Vector<3>& up, const Vector<3>& direction, const Vector<3>& position)
	{
		Matrix<4, 4> rotation_mat
		{
			right.x, up.x, direction.x, 0,
			right.y, up.y, direction.y, 0,
			right.z, up.z, direction.z, 0,
			0, 0, 0, 1
		};
	
		Matrix<4, 4> translation_mat
		{
			1, 0, 0, -position.x,
			0, 1, 0, -position.y,
			0, 0, 1, -position.z,
			0, 0, 0, 1
		};

		Matrix<4, 4> res = rotation_mat * translation_mat;
		res.data_rc[1][1] *= -1;

		return res;
	}

	CML_NO_DISCARD
	CML_FUNC Matrix<4, 4> transpose(const Matrix<4, 4>& other)
	{
		Matrix<4, 4> res{};
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				res.data_rc[i][j] = other.data_rc[j][i];
			}
		}
		return res;
	}
	
	using Mat3 = Matrix<3, 3>;
	using Mat4 = Matrix<4, 4>;
	
	using V3 = Vector<3>;
	using V4 = Vector<4>;
	
	using M3 = Matrix<3, 3>;
	using M4 = Matrix<4, 4>;
}