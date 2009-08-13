

#ifndef OBJ_GEOMETRY
#define OBJ_GEOMETRY

#include <map>
#include <string>
#include <vector>

namespace wcl
{
	struct OBJVertex
	{
		int pointIndex;
		int normalIndex;
		int uvIndex;
	}

	struct OBJMaterial
	{
			string name;                // newmtl [NAME]
			Vector diffuse;             // Kd
			Vector ambient;             // Ka
			Vector specular;            // Ks
			Vector opacity;             // Tf
			double refractionIndex;     // Ni
			double specularExp;         // Ns
	};

	struct OBJFace
	{
		OBJMaterial* material;
		std::Vector<OBJVertex> verts;
	}

	struct OBJGeometry
	{
		std::vector<wcl::Vector> points;
		std::vector<wcl::Vector> normals;
		std::vector<wcl::Vector> uvs;
		std::map<std::string, OBJMaterial> materials;
	};
}

#endif

