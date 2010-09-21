/**
 * Spatial AR Framework ~ The Crazy Bruce Lab
 *
 * Copyright (c) 2008-2009 Michael Marner.
 * Copyright (c) 2009 Markus Broecker
 *
 * If you use/extend/modify this code, add your name and email address
 * to the AUTHORS file in the root directory.
 *
 * This code can be used by members of the Wearable Computer Lab, University
 * of South Australia for research purposes. Commercial use is not allowed
 * without written permission. This copyright notice is subject to change.
 *
 */


#ifndef GEOMETRY_LOADER_H
#define GEOMETRY_LOADER_H

#include <map>
#include <string>

#include <wcl/api.h>
#include "PolygonObject.h"

namespace wcl
{
	/**
	 * Provides static functions for loading geometry for drawing.
	 */
	class WCL_API GeometryLoader
	{
		public:
			/**
			 * Loads a PolygonObject from an OBJ file.
			 *
			 * Note that this is a simlified version that ignores UVs and vertex normals.
			 * This is because CSG operations require a solid object, so vertices shouldn't
			 * be at the same location but with different UV/normals.
			 *
			 * This may change in the future.
			 *
			 * @param path The name of the obj file to load.
			 *
			 */
			static PolygonObject* loadSolidFromOBJ(std::string path);

	};
}

#endif

