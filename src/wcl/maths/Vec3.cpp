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

