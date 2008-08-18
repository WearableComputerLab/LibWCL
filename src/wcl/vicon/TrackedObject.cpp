#include <math.h>

#include "TrackedObject.h"

TrackedObject::TrackedObject(std::string name)
{
	this->name = name;
	
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

