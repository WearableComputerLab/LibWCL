/*-
 * Copyright (c) 2011 Markus Broecker <markus.broecker@unisa.edu.au>
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

#ifndef BOUNDINGSPHERE_H_W9FTZD69
#define BOUNDINGSPHERE_H_W9FTZD69

#include <wcl/api.h>
#include <wcl/maths/Vector.h>

namespace wcl
{
	
class BoundingBox;

class WCL_API BoundingSphere
{
	public:	
		Vector		center;
		double		radius;
		
		BoundingSphere(const Vector& center, double radius);
		
		/// \name Adding stuff
		/// \{
		
		void addPoint(const Vector& point);
		void addBox(const BoundingBox& box);
		void addSphere(const BoundingSphere& sphere);
		
		/// \}
		
		/// \name Query
		/// \{
		
		bool overlaps(const BoundingSphere& shere) const;
		bool contains(const Vector& point) const;
		bool contains(const BoundingSphere& sphere) const;
		
		/// \}
};
	
}; /* wcl */


#endif /* end of include guard: BOUNDINGSPHERE_H_W9FTZD69 */
