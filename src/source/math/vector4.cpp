#include "../../include/core/math/vector4.h"

namespace halogen::math
{
	Vector4::Vector4(): m_x(0.0), m_y(0.0), m_z(0.0), m_w(0.0)
	{
	}

	Vector4::Vector4(float value): m_x(value), m_y(value), m_z(value), m_w(value)
	{
	}

	Vector4::Vector4(float x, float y, float z, float w): m_x(x), m_y(y), m_z(z), m_w(w)
	{
	}

	Vector4::Vector4(const Vector4& other)
	{
		m_x = other.m_x;
		m_y = other.m_y;
		m_z = other.m_z;
		m_w = other.m_w;
	}

	Vector4& Vector4::operator+(const Vector4& other)
	{
		this->m_x = m_x +  other.m_x;
		this->m_y = m_y + other.m_y;
		this->m_z = m_z + other.m_z;
		this->m_w = m_w + other.m_w;

		return *this;
	}

	Vector4& Vector4::operator-(const Vector4& other)
	{
		this->m_x = m_x - other.m_x;
		this->m_y = m_y - other.m_y;
		this->m_z = m_z - other.m_z;
		this->m_w = m_w - other.m_w;

		return *this;
	}

	Vector4& Vector4::operator*(const Vector4& other)
	{
		this->m_x = m_x * other.m_x;
		this->m_y = m_y * other.m_y;
		this->m_z = m_z * other.m_z;
		this->m_w = m_w * other.m_w;

		return *this;
	}

	Vector4& Vector4::operator/(const Vector4& other)
	{
		this->m_x = m_x / other.m_x;
		this->m_y = m_y / other.m_y;
		this->m_z = m_z / other.m_z;
		this->m_w = m_w / other.m_w;

		return *this;
	}

	Vector4& Vector4::operator=(const Vector4& other)
	{
		this->m_x = other.m_x;
		this->m_y = other.m_y;
		this->m_z = other.m_z;
		this->m_w = other.m_w;

		return *this;
	}

	Vector4 Vector4::operator-()
	{
		Vector4 result;
		result.m_x = -m_x;
		result.m_y = -m_y;
		result.m_z = -m_z;
		result.m_w = -m_w;

		return result;
	}

	void Vector4::operator+=(const Vector4& other)
	{
		this->m_x += other.m_x;
		this->m_y += other.m_y;
		this->m_z += other.m_z;
		this->m_w += other.m_w;
	}

	void Vector4::operator-=(const Vector4& other)
	{
		this->m_x -= other.m_x;
		this->m_y -= other.m_y;
		this->m_z -= other.m_z;
		this->m_w -= other.m_w;
	}
	void Vector4::operator*=(const Vector4& other)
	{
		this->m_x *= other.m_x;
		this->m_y *= other.m_y;
		this->m_z *= other.m_z;
		this->m_w *= other.m_w;
	}

	void Vector4::operator/=(const Vector4& other)
	{
		this->m_x /= other.m_x;
		this->m_y /= other.m_y;
		this->m_z /= other.m_z;
		this->m_w /= other.m_w;
	}

	Vector4 Vector4::operator*(float scalar)
	{
		return Vector4(m_x * scalar, m_y * scalar, m_z * scalar, m_w * scalar);
	}

	float Vector4::length()
	{
		float length = 0;
		length = m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w;
		length = sqrtf(length);

		return length;
	}

	float Vector4::length_squared()
	{
		float length_squared = m_x * m_x + m_y * m_y + m_z * m_z + m_w * m_w;
		return length_squared;
	}

	Vector4 Vector4::get_normalized()
	{
		float len = length();
		Vector4 vector = *this;

		return vector / len;
	}

	void Vector4::normalize()
	{
		float len = length();
		*this = *this / len;
	}

	float difference(const Vector4& a, const Vector4& b)
	{
		Vector4 difference = a;
		difference -= b;

		return difference.length();
	}

	float dot_product(const Vector4& a, const Vector4& b)
	{
		float result = (a.m_x * b.m_x + a.m_y * b.m_y + a.m_z * b.m_z + a.m_w * b.m_w);
		return result;
	}

	std::ostream& operator<<(std::ostream& out, Vector4& vector)
	{
		out << "(" << vector.m_x << ", " << vector.m_y << ", " << vector.m_z << ", " << vector.m_w << ")";
		return out;
	}
}