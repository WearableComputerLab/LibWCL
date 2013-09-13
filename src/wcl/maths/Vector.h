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

#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>

#include <wcl/api.h>
#include <wcl/maths/Matrix.h>

namespace wcl {

/**
 * A Class representing a vector (aka a 1D matrix)
 */
class WCL_API Vector : public Matrix
{
public:
    Vector();
    Vector( const Matrix & );
    Vector( unsigned size );
    Vector( const Vector & );
	Vector(T x, T y);
	Vector(T x, T y, T z);
	Vector(T x, T y, T z, T w);

    void setSize( unsigned );

    virtual ~Vector();

    T &operator[] ( unsigned );
    const T &operator[] ( unsigned ) const;

    Vector operator + ( const Vector & ) const;
    Vector operator - ( const Vector & ) const;
    Vector operator - () const;
    Vector operator * ( const T & ) const;

    T  operator * ( const Vector & ) const;
    Vector operator / ( const  T & ) const;

    Vector & operator = ( const Vector & );
    Vector & operator +=( const Vector & );
    Vector & operator -=( const Vector & );
    Vector & operator *=( const T & );
    Vector & operator /=( const T & );


	inline T length() const { return this->normal(); }
	inline T lengthSquared() const { return this->dot(*this); }

    T normal() const;
    Vector unit() const;
	Vector crossProduct(const Vector& v) const;
	T angle(const Vector& v) const;

	T dot(const Vector&) const;

private:
    using Matrix::setSize;
    using Matrix::getCols;
};

// Global Operators
Vector WCL_API operator *(const T &, const Vector & );
Vector WCL_API operator *(const Matrix &, const Vector & );

}; //namespace wcl

inline std::ostream& operator << (std::ostream& os, const wcl::Vector& v)
{
        return os << "[" << v[0] << "," << v[1] << "," << v[2] << "]";
}

#endif
