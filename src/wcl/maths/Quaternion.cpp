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
#include <math.h>

#include <assert.h>

namespace wcl
{

	Quaternion::Quaternion(T _w, T _x, T _y, T _z) : m_X(_x), m_Y(_y), m_Z(_z), m_W(_w)
	{
		//helpimtrappedinauniversefactory
		assert(m_W != 0);
	}

	Quaternion::Quaternion(const wcl::Quaternion& q)
	{
		m_X = q.m_X;
		m_Y = q.m_Y;
		m_Z = q.m_Z;
		m_W = q.m_W;
	}

	Quaternion::Quaternion() : m_X(0), m_Y(0), m_Z(0), m_W(1)
	{
	}

	Quaternion& Quaternion::operator=(const wcl::Quaternion& rhs)
	{
		m_X = rhs.m_X;
		m_Y = rhs.m_Y;
		m_Z = rhs.m_Z;
		m_W = rhs.m_W;
	}

	/**
	 * Constructs a quaternion from an axis and angle of rotation.
	 *
	 * @param axis A vector that is the axis of rotation
	 * @param angle The angle to rotate, in radians
	 */
	Quaternion::Quaternion(const wcl::Vector& axis, T angle)
	{
		this->m_W = cos(angle/2.0);
		T scale = sin(angle/2.0);

		wcl::Vector v(axis);
		v * scale;
		m_X = v[0];
		m_Y = v[1];
		m_Z = v[2];
	}

	void Quaternion::set(T w, T x, T y, T z)
	{
		this->m_X = x;
		this->m_Y = y;
		this->m_Z = z;
		this->m_W = w;
		assert(m_W != 0);
	}

	Quaternion::Quaternion(const wcl::Vector& v1, const wcl::Vector& v2)
	{
		wcl::Vector r = v1.crossProduct(v2);
		T s = sqrt(2*(1 + v1.dot(v2)));
		this->m_W = s/2.0;
		r = r * (1.0/s);
		this->m_X = r[0];
		this->m_Y = r[1];
		this->m_Z = r[2];
	}

	wcl::SMatrix Quaternion::getRotation() const
	{
		T s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;
		s = 2.0/(m_X*m_X + m_Y*m_Y + m_Z*m_Z + m_W*m_W);

		xs = s*m_X;	ys = s*m_Y;	zs = s*m_Z;
		wx = m_W*xs;	wy = m_W*ys;	wz = m_W*zs;
		xx = m_X*xs;	xy = m_X*ys;	xz = m_X*zs;
		yy = m_Y*ys;	yz = m_Y*zs;	zz = m_Z*zs;

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
		double vMult = 2.0 * (m_X*v[0] + m_Y*v[1] + m_Z*v[2]);
		double crossMult = 2.0*m_W;
		double pMult = crossMult*m_W - 1.0;

		return wcl::Vector(pMult*v[0] + vMult*m_X + crossMult*(m_Y*v[2] - m_Z*v[1]),
						   pMult*v[1] + vMult*m_Y + crossMult*(m_Z*v[0] - m_X*v[2]),
						   pMult*v[2] + vMult*m_Z + crossMult*(m_X*v[1] - m_Y*v[0]));
	}



	std::string Quaternion::toString()
	{
		std::stringstream ss;
		ss << "w: " << m_W << " x: " << m_X << " y: " << m_Y << " z: " << m_Z;
		return ss.str();
	}

	T Quaternion::x() const { return m_X;}
	T Quaternion::y() const { return m_Y;}
	T Quaternion::z() const { return m_Z;}
	T Quaternion::w() const { return m_W;}
}

