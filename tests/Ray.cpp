#include <gtest/gtest.h>

#include <iostream>

#include <wcl/geometry/Ray.h>
#include <wcl/maths/Vector.h>
#include <wcl/Exception.h>

#include <cmath>

// The fixture for testing wcl::Ray. 
class RayTest : public ::testing::Test {
};

TEST_F(RayTest, constructor) {

    wcl::Ray ray(wcl::Vector(0,0,0), wcl::Vector(0,1,0));

    ASSERT_EQ(wcl::Vector(0,0,0), ray.getStart());
    ASSERT_EQ(wcl::Vector(0,1,0), ray.getDirection());
}

TEST_F(RayTest, RayRayIntersection) {

    wcl::Ray rayX(wcl::Vector(1, 0, 0), wcl::Vector(0,1,0));
    wcl::Ray rayY(wcl::Vector(0, 1, 0), wcl::Vector(1,0,0));

    wcl::Intersection ip;
    ip.intersects = wcl::Intersection::YES;
    ip.point = wcl::Vector(1,1,0);

    ASSERT_EQ(ip, rayX.intersect(rayY));
    ASSERT_EQ(ip, rayY.intersect(rayX));

    ip.point = wcl::Vector(1,0,0);    
    ASSERT_NE(ip, rayX.intersect(rayY));
    ASSERT_NE(ip, rayY.intersect(rayX));
}

TEST_F(RayTest, isOnRay) {
    
    wcl::Ray rayX(wcl::Vector(1,0,0), wcl::Vector(0,1,0));
    wcl::Vector onRay(1,1,0);
    wcl::Vector offRay(2,0,0);

    ASSERT_TRUE(rayX.isOnRay(onRay)) << onRay << " should be on ray " << rayX;
    ASSERT_FALSE(rayX.isOnRay(offRay)) << offRay << " should not be on ray " << rayX;
}
