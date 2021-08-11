#pragma once

#include <iostream>
#include <ostream>
#include <cmath>

namespace halogen::math
{
    //Note : Will be using templates later
    class Vector3
    {
    public:
        Vector3();
        Vector3(float value);
        Vector3(float x, float y, float z);

        Vector3(const Vector3& other);

        ~Vector3() = default;

        //Operator overloads
        Vector3& operator+(const Vector3& other);
        Vector3& operator-(const Vector3& other);
        Vector3& operator*(const Vector3& other);
        Vector3& operator/(const Vector3& other);

        Vector3 operator-();

        Vector3& operator=(const Vector3& other);

        void operator+=(const Vector3& other);
        void operator-=(const Vector3& other);
        void operator*=(const Vector3& other);
        void operator/=(const Vector3& other);

        Vector3 operator*(float scalar);

        float length();
        float length_squared();

        Vector3 get_normalized();
        void normalize();

        //Friend functions
        friend float dot_product(const Vector3& a, const Vector3& b);
        friend Vector3 cross_product(const Vector3& a, const Vector3& b);

        friend std::ostream& operator<<(std::ostream& out, Vector3& vector);

    public:
    	struct
		{
			float m_x;
			float m_y;
			float m_z;
		};
    };

    using Vector3f = Vector3;
}