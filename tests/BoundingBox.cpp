#include <gtest/gtest.h>

#include <iostream>

#include <wcl/geometry/Ray.h>
#include <wcl/geometry/Line.h>
#include <wcl/geometry/BoundingBox.h>
#include <wcl/maths/Vector.h>
#include <wcl/Exception.h>

#include <cmath>

// The fixture for testing wcl::BoundingBox. 
class BoundingBoxTest : public ::testing::Test {
};

TEST_F(BoundingBoxTest, constructor) {

    wcl::BoundingBox bbox(wcl::Vector(0,0,0), wcl::Vector(1,1,1));

    ASSERT_EQ(wcl::Vector(0.5,0.5,0.5), bbox.getCentroid());
    ASSERT_TRUE(bbox.contains(wcl::Vector(0.5,0.5,0.5)));
}

TEST_F(BoundingBoxTest, intersections) {
    wcl::BoundingBox bbox(wcl::Vector(0,0,0), wcl::Vector(5,5,5));
    wcl::Line l(wcl::Vector(-10,0,0), wcl::Vector(0,1,0));
    wcl::Ray r(wcl::Vector(-10,0,0), wcl::Vector(0,1,0));
    ASSERT_TRUE(bbox.intersect(l));
    ASSERT_TRUE(bbox.intersect(r));
}
