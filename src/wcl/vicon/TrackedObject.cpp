#include <math.h>
#include <sstream>

#include "TrackedObject.h"


using namespace vicon;

TrackedObject::TrackedObject(std::string name, ObjectType type)
{
	this->name = name;
	this->type = type;
	
	x = 0;
	y = 0;
	z = 0;
	rx = 0;
	ry = 0;
	rz = 0;
	
}

TrackedObject::~TrackedObject()
{
}

std::string TrackedObject::getName()
{
	return name;
}

std::string TrackedObject::toString() {
	std::stringstream s;
	s << name;
	s << ": (" ;
	s << x ;
	s << ", "; 
	s << y; 
	s << ", ";
	s << z;
	s << ") (";
	s << rx << ", " << ry << ", " << rz << ")";
	return s.str(); 	
	
}

void TrackedObject::updateData(double* array, int &offset)
{
	//Translation is common to both types
	x = array[offset++];
	y = array[offset++];
	z = array[offset++];

	//The marker has an occluded value
	if (this->type == MARKER)
	{
		double o = array[offset++];
		if (o > 0)
			occluded = true;
		else
			occluded = false;
	}

	//Six dof objects also have rotation
	else if (this->type == SIX_DOF)
	{
		x = array[offset++];
		y = array[offset++];
		z = array[offset++];

		rx = array[offset++];
		ry = array[offset++];
		rz = array[offset++];
	}
}


