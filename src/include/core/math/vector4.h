#pragma once

#include <iostream>
#include <ostream>
#include <cmath>

namespace halogen::math
{
	//Note : Will be using templates later
	class Vector4
	{
	public:
		Vector4();
		Vector4(float value);
		Vector4(float x, float y, float z, float w);

		Vector4(const Vector4& other);

		~Vector4() = default;

		//Operator overloads
		Vector4& operator+(const Vector4& other);
		Vector4& operator-(const Vector4& other);
		Vector4& operator*(const Vector4& other);
		Vector4& operator/(const Vector4& other);

		Vector4 operator-();

		Vector4& operator=(const Vector4& other);

		void operator+=(const Vector4& other);
		void operator-=(const Vector4& other);
		void operator*=(const Vector4& other);
		void operator/=(const Vector4& other);

		Vector4 operator*(float scalar);

		float length();
		float length_squared();

		Vector4 get_normalized();
		void normalize();

		//Friend functions
		friend float dot_product(const Vector4& a, const Vector4& b);

		friend std::ostream& operator<<(std::ostream& out, Vector4& vector);

	public:
		struct
		{
			float m_x;
			float m_y;
			float m_z;
			float m_w;
		};
	};

	using Vector4f = Vector4;
}