/*
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


#include <wcl/maths/Quaternion.h>
#include <cmath>

#include <cassert>

template<typename T>
static inline const T& max(const T& a, const T& b)
{
	return (a > b ? a : b);
}

namespace wcl
{

	Quaternion::Quaternion(T _w, T _x, T _y, T _z) : w(_w), x(_x), y(_y), z(_z) 
	{
		//helpimtrappedinauniversefactory
		//assert(m_W != 0);
	}


	Quaternion::Quaternion() : w(1.0), x(1.0), y(1.0), z(1.0)
	{
	}

	Quaternion::Quaternion(const SMatrix& m)
	{
		assert (m.getRows() >= 3);
		
		w = sqrt( max( 0.0, 1 + m[0][0] + m[1][1] + m[2][2] ) ) / 2;
		x = sqrt( max( 0.0, 1 + m[0][0] - m[1][1] - m[2][2] ) ) / 2;
		y = sqrt( max( 0.0, 1 - m[0][0] + m[1][1] - m[2][2] ) ) / 2;
		z = sqrt( max( 0.0, 1 - m[0][0] - m[1][1] + m[2][2] ) ) / 2;
		                                             
		x = copysign( x, m[2][1] - m[1][2] );
		y = copysign( y, m[0][2] - m[2][0] );
		z = copysign( z, m[1][0] - m[0][1] );
	}

	/**
	 * Constructs a quaternion from an axis and angle of rotation.
	 *
	 * @param axis A vector that is the axis of rotation
	 * @param angle The angle to rotate, in radians
	 */
	Quaternion::Quaternion(const wcl::Vector& axis, T angle)
	{
		w = cos(angle/2.0);
		T scale = sin(angle/2.0);

		wcl::Vector v(axis);
		v * scale;
		x = v[0];
		y = v[1];
		z = v[2];
	}

	Quaternion::Quaternion(const wcl::Vector& v1, const wcl::Vector& v2)
	{
		wcl::Vector r = v1.crossProduct(v2);
		T s = sqrt(2*(1 + v1.dot(v2)));
		this->w = s/2.0;
		r = r * (1.0/s);
		this->x = r[0];
		this->y = r[1];
		this->z = r[2];
	}

	wcl::SMatrix Quaternion::getRotation() const
	{
		T s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;
		s = 2.0/(x*x + y*y + z*z + w*w);

		xs = s*x;	ys = s*y;	zs = s*z;
		wx = w*xs;	wy = w*ys;	wz = w*zs;
		xx = x*xs;	xy = x*ys;	xz = x*zs;
		yy = y*ys;	yz = y*zs;	zz = z*zs;

		wcl::SMatrix m(4);

		m[0][0] = 1.0 - (yy+zz);
		m[0][1] = xy - wz;
		m[0][2] = xz + wy;

		m[1][0] = xy + wz;
		m[1][1] = 1.0 - (xx + zz);
		m[1][2] = yz - wx;

		m[2][0] = xz - wy;
		m[2][1] = yz + wx;
		m[2][2] = 1.0 - (xx + yy);

		m[3][3] = 1;

		return m;
	}

	wcl::Vector Quaternion::rotate(const wcl::Vector& v) const
	{
		double vMult = 2.0 * (x*v[0] + y*v[1] + z*v[2]);
		double crossMult = 2.0*w;
		double pMult = crossMult*w - 1.0;

		return wcl::Vector(pMult*v[0] + vMult*x + crossMult*(y*v[2] - z*v[1]),
						   pMult*v[1] + vMult*y + crossMult*(z*v[0] - x*v[2]),
						   pMult*v[2] + vMult*z + crossMult*(x*v[1] - y*v[0]));
	}

	wcl::Quaternion Quaternion::rotate(wcl::Quaternion q) const
	{
		// this follows closely realtime rendering, 2nd ed. pg75ff
		return *this * q * this->getConjugate();
	}
	
	wcl::Quaternion Quaternion::operator * (const Quaternion& r) const
	{
		// this follows closely realtime rendering, 2nd ed. pg72ff
		
		wcl::Vector qv = wcl::Vector(x, y, z);
		wcl::Vector rv = wcl::Vector(r.x, r.y, r.z);
		
		wcl::Vector vr = qv.crossProduct(rv) + r.w*qv + w*rv;
				
		double W = this->w * r.w - qv.dot(rv);
		return wcl::Quaternion(W, vr[0], vr[1], vr[2]);
	}


	std::string Quaternion::toString()
	{
		std::stringstream ss;
		ss << "w: " << w << " x: " << x << " y: " << y << " z: " << z;
		return ss.str();
	}
	

}

