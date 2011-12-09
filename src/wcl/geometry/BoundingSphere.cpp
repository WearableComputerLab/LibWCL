#include "BoundingSphere.h"
#include "BoundingBox.h"

#include <cmath>

using namespace wcl;

BoundingSphere::BoundingSphere(const Vector& c, double r) : center(c), radius(r)
{
}

void BoundingSphere::addPoint(const Vector& pt)
{
	double d2 = (pt-center).lengthSquared(); 
	if (d2 < radius*radius)
		radius += sqrt(d2);
}

void BoundingSphere::addBox(const BoundingBox& box)
{
	addPoint(box.min);
	addPoint(box.max);
}

void BoundingSphere::addSphere(const BoundingSphere& sphere)
{
	double d = (sphere.center - center).length();
	radius = std::max(radius, d+sphere.radius);
}

bool BoundingSphere::overlaps(const BoundingSphere& sphere) const
{
	double d2 = (sphere.center - center).lengthSquared();
	return (d2 < radius*radius) || (d2 < sphere.radius*sphere.radius);
}

bool BoundingSphere::contains(const Vector& point) const
{
	double d2 = (point-center).lengthSquared();
	return (d2 < radius*radius);
}

bool BoundingSphere::contains(const BoundingSphere& other) const
{
	
}
	