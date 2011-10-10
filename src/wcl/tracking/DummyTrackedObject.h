/*
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

#ifndef DUMMYTRACKEDOBJECT_H_I20M3SIJ
#define DUMMYTRACKEDOBJECT_H_I20M3SIJ

#include <wcl/tracking/TrackedObject.h>
#include <wcl/maths/Vector.h>
#include <wcl/maths/SMatrix.h>

namespace wcl
{
class DummyTrackedObject : public TrackedObject
{
public:
	DummyTrackedObject(const std::string& name, const Vector& position);
	
	/// \name Public interface implementation
	/// \{
	
	inline std::string toString() const { return "DummyTrackedObject '" + name + "'"; }
	SMatrix getTransform() const;
	Vector getTranslation() const;
	Quaternion getOrientation() const;
	inline bool isVisible() const { return true; }
	
	/// \}
	
	/// TODO: add methods for setting position, looking at stuff, etc
	
private:
	Vector		position;
};

	
} /* wcl */


#endif /* end of include guard: DUMMYTRACKEDOBJECT_H_I20M3SIJ */
