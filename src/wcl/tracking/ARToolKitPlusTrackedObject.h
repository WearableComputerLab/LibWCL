/*-
 * Copyright (c) 2010 Benjamin Close <Benjamin.Close@clearchain.com>
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

#ifndef WCL_TRACKING_ARTOOLKITPLUSTRACKEDOBJECT_H
#define WCL_TRACKING_ARTOOLKITPLUSTRACKEDOBJECT_H

#include <wcl/api.h>
#include <wcl/maths/SMatrix.h>
#include <wcl/tracking/TrackedObject.h>

namespace wcl {

	class WCL_API ARToolKitPlusTrackedObject: public TrackedObject
	{
		public:
			ARToolKitPlusTrackedObject(const unsigned width, const unsigned id);

			virtual ~ARToolKitPlusTrackedObject();
			virtual std::string toString() const;
			virtual SMatrix getTransform() const;
			virtual Vector getTranslation() const;
			virtual Quaternion getOrientation() const;
			virtual unsigned getID() const;

			void setTransform(const SMatrix &);
			void setVisible( const bool state);
			void setConfidence( const float value );

			virtual bool isVisible() const;
			virtual bool hasBeenSeen() const;
			unsigned getWidth() const;

		private:
			unsigned id;
			unsigned width;
			bool visible;
			bool seen;

			SMatrix transform;
	};

};

#endif // WCL_TRACKING_ARTOOLKTIPLUSTRACKEDOBJECT_H
