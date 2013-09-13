/*-
 * Copyright (c) 2004-2008 Benjamin Close <Benjamin.Close@clearchain.com>
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

#include <assert.h>
#include <math.h>
#include "Vector.h"

namespace wcl {

/**
 * DefaultConstructor:
 */
Vector::Vector()
{}

/**
 * Allocate a vector of the given size
 *
 * @param size The size to allocate the vector to be
 */
Vector::Vector( unsigned size )
{
    this->setSize( size );
}

/**
 * Convenience constructor for creating a 2 value vector.
 * This can be used for storing x,y coordinates, etc.
 */
Vector::Vector(T x, T y)
{
	this->setSize(2);
	Vector::operator[] (0) = x;
	Vector::operator[] (1) = y;
}

/**
 * Convenience constructor for creating a 3 value vector.
 * This can be used for storing x,y,z coordinates, colours, etc.
 */
Vector::Vector(T x, T y, T z)
{
	this->setSize(3);
	Vector::operator[] (0) = x;
	Vector::operator[] (1) = y;
	Vector::operator[] (2) = z;
}

/**
 * Convenience constructor for creating a 3 value vector.
 * This can be used for storing x,y,z coordinates, colours, etc.
 */
Vector::Vector(T x, T y, T z, T w)
{
	this->setSize(4);
	Vector::operator[] (0) = x;
	Vector::operator[] (1) = y;
	Vector::operator[] (2) = z;
	Vector::operator[] (3) = w;
}

Vector::Vector( const Matrix &m ):
    Matrix( m)
{
    assert( m.getCols() == 1 && "Cant represent the matrix as a vector");
}

/**
 * Copy Constructor
 *
 * @param v The vector to copy
 */

Vector::Vector( const Vector &v ):
    Matrix( v )
{}

/**
 * Destructor
 */
Vector::~Vector()
{}

/**
 * Set the size of the vector
 *
 * @param size The size to set the vector to be
 */
void Vector::setSize( unsigned size)
{
    Matrix::setSize( size, 1 );
}

/**
 * Obtain the requested Element from the vector for rhs evaluation
 *
 * @return The requested element
 */
T &Vector::operator[] ( unsigned index)
{
    assert ( index < this->getRows() && "Invalid Vector index");

    return Matrix::operator [](index)[0];

}

/**
 * Obtain the requested element from the vector
 *
 * @return The requested element
 */
const T &Vector::operator[] ( unsigned index) const
{
    assert ( index < this->getRows() && "Invalid Vector (const) index" );

    return Matrix::operator[](index)[0];
}

/**
 * Sum two vectors together
 *
 * @param v The vector to sum this vector with
 */
Vector Vector::operator + ( const Vector &v ) const
{
    Vector myvec ( *this );

    myvec += v;

    return myvec;
}

/**
 * Substract one vector from the other
 *
 * @param v The vector to subtract from the current
 */
Vector Vector::operator - ( const Vector &v ) const
{
    Vector myvec ( *this );

    myvec -= v;

    return myvec;
}

/**
 * Negation of the vector
 */
Vector Vector::operator - () const
{
    Vector myvec ( *this );

    myvec *= -1.0;

    return myvec;
}

/**
 * Multiplication of the vector by the constant
 *
 * @param value The value to multiply the vector by
 */
Vector Vector::operator * ( const T &value ) const
{
    Vector myvec ( *this );

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
T Vector::operator * ( const Vector & v) const
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
Vector Vector::operator / ( const T &value ) const
{
    Vector myvec ( *this );

    myvec /= value;

    return myvec;
}

/**
 * Perform Assignment of one vector to another
 *
 * @param v The vector to assign too
 */
Vector & Vector::operator = ( const Vector &v )
{
    if ( *this == v ){
	return *this;
    }

    return (Vector &)Matrix::operator = (v);
}


/**
 * Addition assignment
 *
 * @param v The vector to use for assignment
 */
Vector & Vector::operator +=( const Vector &v)
{
    return (Vector &)Matrix::operator += ( v );
}


/**
 * Subtractional Assignment
 *
 * @param v The vector to use for assignment
 */
Vector & Vector::operator -=( const Vector &v )
{
    return (Vector &)Matrix::operator -= ( v );
}

/**
 * Mulitiplication Assignment
 *
 * @param value The value to multiply the vector by
 */
Vector & Vector::operator *=( const T & value )
{
    return (Vector &)Matrix::operator *= ( value );
}

/**
 * Division Assingment
 *
 * @param value The value to divide by
 */
Vector & Vector::operator /=( const T &v )
{
    return (Vector &)Matrix::operator /= ( v );
}

/**
 * Obtain the normal of the vector
 */
T Vector::normal() const
{
    T result;

    result = (*this) * (*this );

    return sqrt ( result );
}

/**
 * Obtain the unit vector of this vector
 */
Vector Vector::unit() const
{
    Vector v( *this );

    return v / v.normal();
}


//
// Global Operators
//

/**
 * Multiplication of a vector and a constant
 *
 * @param value The value to multiply the vector by
 * @param v The vector to multpliy
 */
Vector operator *(const T &value, const Vector &v )
{
    Vector myvec ( v );
    myvec *= value;

    return myvec;
}

/**
 * Obtain the vector result of multiplication of the given matrix.
 * Calculation is done by: m * v
 *
 * @param m The matrix to multiply the vector by
 * @param v The vector to multiply;
 */
Vector operator *(const Matrix &m, const Vector &v )
{
    Vector temp ( m.getRows());

    temp.storeProduct( m, v );

    return temp;
}

/**
 * Obtain the dot product of two vectors.
 * Calculates the dot product of this with v, such that
 *
 * return = this DOT v
 *
 * @return the dot product.
 */
T Vector::dot(const Vector& v) const
{
	assert (this->getRows() == v.getRows() && "Vectors must have the same number of values for dot product");
	return operator * (v);
	
}

/**
 * Calculates the angle, in radians, between two vectors.
 *
 * @param v The other vector.
 * @return the angle between the vectors.
 */
T Vector::angle(const Vector& v) const
{
	if (v == *this)
		return 0;

	Vector v1 = this->unit();
	Vector v2 = v.unit();

	return acos(v1.dot(v2));
}

Vector Vector::crossProduct(const Vector& v) const
{
	assert ((this->getRows() ==  3 && v.getRows() == 3) && "Vectors must have length 3 for cross product");
	return Vector((*this)[1] * v[2] - (*this)[2] * v[1], 
				  (*this)[2] * v[0] - (*this)[0] * v[2], 
				  (*this)[0] * v[1] - (*this)[1] * v[0]);
}

/**
 * Find the distance between this and v
 */
float Vector::distance(const Vector& v) const 
{
    return ((*this)-v).length();
}


}; //namespace wcl
