#pragma once

#include <iostream>
#include <ostream>
#include <cmath>

namespace halogen::math
{
	//Note : Will be using templates later
	class Vector2
	{
	public:
		Vector2();
		Vector2(float value);
		Vector2(float x, float y);

		Vector2(const Vector2& other);

		~Vector2() = default;

		//Operator overloads
		Vector2& operator+(const Vector2& other);
		Vector2& operator-(const Vector2& other);
		Vector2& operator*(const Vector2& other);
		Vector2& operator/(const Vector2& other);

		Vector2 operator-();

		Vector2& operator=(const Vector2& other);

		void operator+=(const Vector2& other);
		void operator-=(const Vector2& other);
		void operator*=(const Vector2& other);
		void operator/=(const Vector2& other);

		Vector2 operator*(float scalar);

		float length();
		float length_squared();

		Vector2 get_normalized();
		void normalize();

		//Friend functions
		friend float distance(const Vector2& a, const Vector2& b);

		friend float dot_product(const Vector2& a, const Vector2& b);

		friend std::ostream& operator<<(std::ostream& out, Vector2& vector);

	public:
		struct
		{
			float m_x;
			float m_y;
		};
	};

	using Vector2f = Vector2;
}