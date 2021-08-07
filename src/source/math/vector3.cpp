#include "../../include/core/math/vector3.h"

namespace halogen::math
{
    Vector3::Vector3(): m_x(0.0), m_y(0.0), m_z(0.0)
    {
    }

    Vector3::Vector3(float value): m_x(value), m_y(value), m_z(value)
    {
    }

    Vector3::Vector3(float x, float y, float z): m_x(x), m_y(y), m_z(z)
    {
    }

    Vector3::Vector3(const Vector3& other)
    {
        m_x = other.m_x;
        m_y = other.m_y;
        m_z = other.m_z;
    }

    Vector3& Vector3::operator+(const Vector3& other)
    {
        this->m_x = m_x +  other.m_x;
        this->m_y = m_y + other.m_y;
        this->m_z = m_z + other.m_z;

        return *this;
    }

    Vector3& Vector3::operator-(const Vector3& other)
    {
        this->m_x = m_x - other.m_x;
        this->m_y = m_y - other.m_y;
        this->m_z = m_z - other.m_z;

        return *this;
    }

    Vector3& Vector3::operator*(const Vector3& other)
    {
        this->m_x = m_x * other.m_x;
        this->m_y = m_y * other.m_y;
        this->m_z = m_z * other.m_z;

        return *this;
    }

    Vector3& Vector3::operator/(const Vector3& other)
    {
        this->m_x = m_x / other.m_x;
        this->m_y = m_y / other.m_y;
        this->m_z = m_z / other.m_z;

        return *this;
    }

    Vector3& Vector3::operator=(const Vector3& other)
    {
        this->m_x = other.m_x;
        this->m_y = other.m_y;
        this->m_z = other.m_z;

        return *this;
    }

    void Vector3::operator+=(const Vector3& other)
    {
        this->m_x += other.m_x;
        this->m_y += other.m_y;
        this->m_z += other.m_z;
    }

    void Vector3::operator-=(const Vector3& other)
    {
        this->m_x -= other.m_x;
        this->m_y -= other.m_y;
        this->m_z -= other.m_z;
    }
    void Vector3::operator*=(const Vector3& other)
    {
        this->m_x *= other.m_x;
        this->m_y *= other.m_y;
        this->m_z *= other.m_z;
    }

    void Vector3::operator/=(const Vector3& other)
    {
        this->m_x /= other.m_x;
        this->m_y /= other.m_y;
        this->m_z /= other.m_z;
    }

    float Vector3::length()
    {
        float length = 0;
        length = m_x * m_x + m_y * m_y + m_z * m_z;
        length = sqrtf(length);

        return length;
    }

    float Vector3::length_squared()
    {
        float length_squared = m_x * m_x + m_y * m_y + m_z * m_z;
        return length_squared;
    }

    Vector3 Vector3::get_normalized()
    {
        float len = length();
        Vector3 vector = *this;

        return vector / len;
    }

    void Vector3::normalize()
    {
        float len = length();
        *this = *this / len;
    }


    float dot_product(const Vector3& a, const Vector3& b)
    {
        float result = (a.m_x * b.m_x + a.m_y * b.m_y + a.m_z * b.m_z);
        return result;
    }

    std::ostream& operator<<(std::ostream& out, Vector3& vector)
    {
        out << "(" << vector.m_x << ", " << vector.m_y << ", " << vector.m_z << ")";
        return out;
    }
}