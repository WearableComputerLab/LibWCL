/**
 * Spatial AR Framework ~ The Crazy Bruce Lab
 *
 * Copyright (c) 2008-2009 Michael Marner.
 *
 * If you use/extend/modify this code, add your name and email address
 * to the AUTHORS file in the root directory.
 *
 * This code can be used by members of the Wearable Computer Lab, University
 * of South Australia for research purposes. Commercial use is not allowed
 * without written permission. This copyright notice is subject to change.
 *
 */

#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include <string>
#include <string.h>
#include <wcl/maths/Vector.h>

#include "GeometryLoader.h"


namespace wcl
{
	PolygonObject* GeometryLoader::loadOBJ(string path)
	{
		ifstream in(path.c_str(), ifstream::in);

		if (!in.good())
			throw std::string("Cannot Open File!");

		PolygonObject* obj = new PolygonObject(path);

		std::vector<wcl::Vector> verts;

		while (in.good())
		{
			string cmd;
			in >> cmd;

			if (strcmp(cmd.c_str(), "v") == 0)
			{
				double x,y,z;
				in >> x;
				in >> y;
				in >> z;
				wcl::Vector* v = new wcl::Vector(x,y,z);
				verts.push_back(v);
			}
			else if (strcmp(cmd.c_str(), "f") == 0)
			{
				//get the rest of the line...
				string restOfLine;
				getline(in, restOfLine);

				double v1,v2,v3;
				Face* f = new Face();

				// looks like this: 1/1/1 2/2/2 3/3/3
				int result = sscanf(response, "%lf/%*lf/%*lf %lf/%*lf/%*lf %lf/%*lf/%*lf", &v1, &v2, &v3);



					// -1 because OBJ starts counting from 1
					int vertexIndex = atoi(string(coordinates[2]).c_str()) - 1;
					int texCoordIndex = atoi(string(coordinates[3]).c_str()) -1;
					int normalIndex = atoi(string(coordinates[4]).c_str()) -1;

					c.vertex = (obj->vertexList[vertexIndex]);
					c.texCoord = (obj->texCoordsList[texCoordIndex]);
					c.normal = (obj->normalList[normalIndex]);
					f->coordinates.push_back(c);

					start = coordinates[0].second;
				}
				f->material = currentMat;

				obj->faceList.push_back(f);

			}
		}
		in.close();

		return obj;
	};

}

