#include "Marker.h"

using namespace vicon;


Marker::Marker(std::string name) : TrackedObject(name)
{
}

Marker::~Marker()
{
}

void Marker::updateData(double* array, int & offset)
{
	
	x = array[offset++];
	y = array[offset++];
	z = array[offset++];
	occluded = array[offset++];
	
}

std::string Marker::toString() {
	
	std::string s;
	s = name;
	s+= ": (" ;
	s += x ;
	s += ", "; 
	s += y; 
	s += ", ";
	s += z;
	s += ")";
	return s; 	
	
}
