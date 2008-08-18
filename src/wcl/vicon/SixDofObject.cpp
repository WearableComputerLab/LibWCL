#include "SixDofObject.h"

#include <iostream>
#include <sstream>

using namespace vicon;


SixDofObject::SixDofObject(std::string name) : TrackedObject(name)
{

}

SixDofObject::~SixDofObject()
{
}

void SixDofObject::updateData(double* array, int & offset)
{
	//std::cout << "About to read" << std::endl;
	x = array[offset++];
	y = array[offset++];
	z = array[offset++];
	
	rx = array[offset++];
	ry = array[offset++];
	rz = array[offset++];
	
}

std::string SixDofObject::toString() {
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

