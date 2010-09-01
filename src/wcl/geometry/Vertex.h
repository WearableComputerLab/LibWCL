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


#ifndef VERTEX_H
#define VERTEX_H

#include <set>
#include <string>
#include <wcl/api.h>
#include <wcl/maths/Vector.h>

namespace wcl
{
	/**
	 * Structure of a single vertex.
	 */
	class WCL_API Vertex
	{
		public:
			Vertex(const wcl::Vector& position, const wcl::Vector& normal, const wcl::Vector& texCoord);
			Vertex(const wcl::Vector& position);
			Vertex(const Vertex& v);
			Vertex();

			bool operator==(const Vertex& other) const;

			std::string toString();

			/**
			 * The position of the vertex in 3D space
			 */
			wcl::Vector position;

			/**
			 * The vertex normal
			 */
			wcl::Vector normal;

			/**
			 * The texture coordinate of the vertex 
			 */
			wcl::Vector texCoord;

		private:
			//nothing :(
	};

}

#endif

