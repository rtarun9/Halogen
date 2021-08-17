#include "../../include/core/math/vector2.h"

namespace halogen::math
{
	Vector2::Vector2(): m_x(0.0), m_y(0.0)
	{
	}

	Vector2::Vector2(float value): m_x(value), m_y(value)
	{
	}

	Vector2::Vector2(float x, float y): m_x(x), m_y(y)
	{
	}

	Vector2::Vector2(const Vector2& other)
	{
		m_x = other.m_x;
		m_y = other.m_y;
	}

	Vector2& Vector2::operator+(const Vector2& other)
	{
		this->m_x = m_x +  other.m_x;
		this->m_y = m_y + other.m_y;

		return *this;
	}

	Vector2& Vector2::operator-(const Vector2& other)
	{
		this->m_x = m_x - other.m_x;
		this->m_y = m_y - other.m_y;

		return *this;
	}

	Vector2& Vector2::operator*(const Vector2& other)
	{
		this->m_x = m_x * other.m_x;
		this->m_y = m_y * other.m_y;

		return *this;
	}

	Vector2& Vector2::operator/(const Vector2& other)
	{
		this->m_x = m_x / other.m_x;
		this->m_y = m_y / other.m_y;

		return *this;
	}

	Vector2& Vector2::operator=(const Vector2& other)
	{
		this->m_x = other.m_x;
		this->m_y = other.m_y;

		return *this;
	}

	Vector2 Vector2::operator-()
	{
		Vector2 result;
		result.m_x = -m_x;
		result.m_y = -m_y;

		return result;
	}

	void Vector2::operator+=(const Vector2& other)
	{
		this->m_x += other.m_x;
		this->m_y += other.m_y;
	}

	void Vector2::operator-=(const Vector2& other)
	{
		this->m_x -= other.m_x;
		this->m_y -= other.m_y;
	}
	void Vector2::operator*=(const Vector2& other)
	{
		this->m_x *= other.m_x;
		this->m_y *= other.m_y;
	}

	void Vector2::operator/=(const Vector2& other)
	{
		this->m_x /= other.m_x;
		this->m_y /= other.m_y;
	}

	Vector2 Vector2::operator*(float scalar)
	{
		return Vector2(m_x * scalar, m_y * scalar);
	}

	float Vector2::length()
	{
		float length = 0;
		length = m_x * m_x + m_y * m_y;
		length = sqrtf(length);

		return length;
	}

	float Vector2::length_squared()
	{
		float length_squared = m_x * m_x + m_y * m_y;
		return length_squared;
	}

	Vector2 Vector2::get_normalized()
	{
		float len = length();
		Vector2 vector = *this;

		return vector / len;
	}

	void Vector2::normalize()
	{
		float len = length();
		*this = *this / len;
	}

	float distance(const Vector2& a, const Vector2& b)
	{
		Vector2 difference = a;
		difference -= b;

		return difference.length();
	}

	float dot_product(const Vector2& a, const Vector2& b)
	{
		float result = (a.m_x * b.m_x + a.m_y * b.m_y);
		return result;
	}

	std::ostream& operator<<(std::ostream& out, Vector2& vector)
	{
		out << "(" << vector.m_x << ", " << vector.m_y << ")";
		return out;
	}
}