/*-
 * Copyright (c) 2008 Michael Marner <michael@20papercups.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#include <wcl/maths/Vec3.h>


using namespace wcl;


Vec3::Vec3(T x, T y, T z) : Vector(3)
{
	Vector::operator[] (0) = x;
	Vector::operator[] (1) = y;
	Vector::operator[] (2) = z;
}

Vec3::Vec3() : Vector(3) 
{
	Vector::operator[] (0) = 0;
	Vector::operator[] (1) = 0;
	Vector::operator[] (2) = 0;
}

T Vec3::x() const 
{
	return Vector::operator[] (0);
}

T Vec3::y() const 
{
	return Vector::operator[] (1);
}

T Vec3::z() const 
{
	return Vector::operator[] (2);
}

void Vec3::x(T x)
{
	Vector::operator[] (0) = x;
}

void Vec3::y(T y)
{
	Vector::operator[] (1) = y;
}

void Vec3::z(T z)
{
	Vector::operator[] (2) = z;
}

Vec3 Vec3::crossProduct(const Vec3& v)
{
	return Vec3(y() * v.z() - z() * v.y(), z() * v.x() - x() * v.z(), x() * v.y() - y() * v.x());

}

/**
 * Sum two vectors together
 *
 * @param v The vector to sum this vector with
 */
Vec3 Vec3::operator + ( const Vec3 &v ) const
{
    Vec3 myvec ( *this );

    myvec += v;

    return myvec;
}

/**
 * Substract one vector from the other
 *
 * @param v The vector to subtract from the current
 */
Vec3 Vec3::operator - ( const Vec3 &v ) const
{
    Vec3 myvec ( *this );

    myvec -= v;

    return myvec;
}


/**
 * Negation of the vector
 */
Vec3 Vec3::operator - () const
{
    Vec3 myvec ( *this );

    myvec *= -1.0;

    return myvec;
}

/**
 * Addition assignment
 *
 * @param v The vector to use for assignment
 */
Vec3 & Vec3::operator +=( const Vec3 &v)
{
    return (Vec3 &)Matrix::operator += ( v );
}


/**
 * Subtractional Assignment
 *
 * @param v The vector to use for assignment
 */
Vec3 & Vec3::operator -=( const Vec3 &v )
{
    return (Vec3 &)Matrix::operator -= ( v );
}

/**
 * Multiplication of the vector by the constant
 *
 * @param value The value to multiply the vector by
 */
Vec3 Vec3::operator * ( const T &value ) const
{
    Vec3 myvec ( *this );

    myvec *= value;

    return myvec;
}

/**
 * Returns the dot product of 2 vectors.
 *
 * Thats right! DOT Product
 *
 * @param v The vector to multiply this vector by
 * \warning The dot function below uses this implementation to reduce code duplication.
 */
T Vec3::operator * ( const Vec3 & v) const
{
    T result = 0.0;

    for ( unsigned i = 0; i < this->getRows(); i++ ){
	result += (*this)[i] * v[i];
    }

    return result;
}

/**
 * Divide each element of this vector by the given value
 *
 * @param value The value to divide by
 */
Vec3 Vec3::operator / ( const T &value ) const
{
    Vec3 myvec ( *this );

    myvec /= value;

    return myvec;
}

/**
 * Perform Assignment of one vector to another
 *
 * @param v The vector to assign too
 */
Vec3 & Vec3::operator = ( const Vec3 &v )
{
    if ( *this == v ){
	return *this;
    }

    return (Vec3 &)Matrix::operator = (v);
}

/**
 * Mulitiplication Assignment
 *
 * @param value The value to multiply the vector by
 */
Vec3 & Vec3::operator *=( const T & value )
{
    return (Vec3 &)Matrix::operator *= ( value );
}

/**
 * Division Assingment
 *
 * @param value The value to divide by
 */
Vec3 & Vec3::operator /=( const T &v )
{
    return (Vec3 &)Matrix::operator /= ( v );
}

/**
 * Multiplication of a vector and a constant
 *
 * @param value The value to multiply the vector by
 * @param v The vector to multpliy
 */
Vec3 operator *(const T &value, const Vec3 &v )
{
    Vec3 myvec ( v );
    myvec *= value;

    return myvec;
}

/**
 * Obtain the unit vector of this vector
 */
Vec3 Vec3::unit() const
{
    Vec3 v( *this );

    return v / v.normal();
}
