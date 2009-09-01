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


#include <assert.h>
#include <config.h>
#include <math.h>
#include <sstream>

#include <wcl/geometry/Vertex.h>

namespace wcl
{
	Vertex::Vertex() : position(wcl::Vector(3)), normal(wcl::Vector(3)), texCoord(wcl::Vector(3))
	{
		// hi!
	}

	Vertex::Vertex(const wcl::Vector& position, const wcl::Vector& normal, const wcl::Vector& texCoord)
	{
		assert(position.getRows() == 3);
		assert(normal.getRows() == 3);
		assert(texCoord.getRows() == 3);

		this->position = position;
		this->normal = normal;
		this->texCoord = texCoord;
	}

	Vertex::Vertex(const wcl::Vector& position)
	{
		assert(position.getRows() == 3);

		this->position = position;
	}

	Vertex::Vertex(const Vertex& v)
	{
		assert(v.position.getRows() == 3);
		this->position = v.position;
		this->normal = v.normal;
		this->texCoord = v.texCoord;
	}

	bool Vertex::operator== (const Vertex& other) const
	{
		return (fabs((this->position - other.position).normal()) < TOL);
	}

	std::string Vertex::toString()
	{
		std::stringstream ss;
		ss << "Vertex. (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
		return ss.str();
	}

}

