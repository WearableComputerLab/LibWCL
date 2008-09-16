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


#ifndef OSG_HELPER_H
#define OSG_HELPER_H

namespace wcl
{

	/**
	 * Class contains helpful functions for dealing with OpenSceneGraph 2.
	 * Common things that should be provided for you, but arent.
	 *
	 * @author Michael Marner (michael@20papercups.net)
	 */
	class OSGHelper
	{

		public:

		/**
		 * Returns the number of screens available to openscenegraph.
		 *
		 * On Linux, this number is the total number of screens defined
		 * in xorg.conf. Note that if you are using Xinerama or TwinView
		 * with two monitors, this appears to openscenegraph as a single
		 * screen.
		 *
		 * On Windows, this should simply be the number of monitors you
		 * have connected.
		 *
		 * BIG DISCLAIMER: OpenSceneGraph must be initialised before this
		 * function is called. Creating an osgViewer should be enough, but
		 * this requires further testing (sorry!).
		 *
		 * @return The number of screens on this system.
		 */
		int getNumScreens();


	};

};

#endif

