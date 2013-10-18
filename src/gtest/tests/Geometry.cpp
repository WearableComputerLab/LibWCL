#include "gtest/gtest.h"

#include <iostream>

#include <wcl/geometry/Line.h>
#include <wcl/maths/SMatrix.h>
#include <wcl/maths/Vector.h>
#include <wcl/maths/Quaternion.h>
#include <wcl/geometry/Plane.h>
#include <wcl/geometry/LineSegment.h>
#include <wcl/Exception.h>

#include <cmath>

// The fixture for testing class Project1. From google test primer.
class GeometryTest : public ::testing::Test {
    protected:
        // You can remove any or all of the following functions if its body
        // is empty.

        GeometryTest() {
            // You can do set-up work for each test here.

        }

        virtual ~GeometryTest() {
            // You can do clean-up work that doesn't throw exceptions here.
        }

        // If the constructor and destructor are not enough for setting up
        // and cleaning up each test, you can define the following methods:
        virtual void SetUp() {
            // Code here will be called immediately after the constructor (right
            // before each test).
        }

        virtual void TearDown() {
            // Code here will be called immediately after each test (right
            // before the destructor).
        }

        wcl::SMatrix getRotationMatrix(float theta) {

            double c, s;

            wcl::SMatrix M(4);
         
            // Theta is in degrees. We need to convert to radians, but
            // this will introduce floating-point errors, so do it all-as-one
            c = cos(theta * PI / 180);
            s = sin(theta * PI / 180);

            double l = 0.0; double m = 0.0; double n = 0;
            M[0][0] = c + (1-c)*l*l;
            M[1][0] =     (1-c)*l*m + s*(-n);
            M[2][0] =     (1-c)*l*n + s*m;
            M[0][1] =     (1-c)*m*l + s*n;
            M[1][1] = c + (1-c)*m*m;
            M[2][1] =     (1-c)*m*n + s*(-l);
            M[0][2] =     (1-c)*n*l + s*(-m);
            M[1][2] =     (1-c)*n*m + s*l;
            M[2][2] = c + (1-c)*n*n;

            //make it a 4x4 matrix
            M[3][0] = 0;
            M[3][1] = 0;
            M[3][2] = 0;
            M[0][3] = 0;
            M[1][3] = 0;
            M[2][3] = 0;
            M[3][3] = 1;

            return M;
        }

        // Objects declared here can be used by all tests in the test case for Project1.
        const long double PI = 3.141592653589793238L;
};

/*
 * Test the constructor of wcl::Line. 
 */
TEST_F(GeometryTest, LineConstructor) {

    wcl::Vector v(0,0,0);
    wcl::Vector w(1,1,1);

    wcl::Line l(v,w);

    ASSERT_EQ(l.getPosition(), v);
    ASSERT_EQ(l.getDirection(),w.unit());
}

TEST_F(GeometryTest, LineLineIntersection) {
    wcl::Vector p(0,1,0);
    wcl::Vector q(1,1,0);
    wcl::Vector r(1,0,0);
    wcl::Vector s(0,0,0);

    wcl::Line pq(p,wcl::Vector( 1, 0, 0));
    wcl::Line ps(p,wcl::Vector( 0,-1, 0));

    wcl::Line qs(q,wcl::Vector( -1,-1,0));
    wcl::Line qr(q,wcl::Vector( 0,-1, 0));

    wcl::Line rs(r,wcl::Vector(-1, 0, 0));
    wcl::Line rp(r,wcl::Vector(-1, 1, 0));

    ASSERT_EQ(pq.intersect(ps), p) << "Lines intersect at p";
    ASSERT_EQ(rs.intersect(rp), r) << "Lines intersect at r";

    ASSERT_EQ(rp.intersect(qs), wcl::Vector(0.5, 0.5, 0)) << "Lines intersect: (0.5, 0.5, 0)";

    ASSERT_THROW(wcl::Vector ps_qr = ps.intersect(qr), wcl::Exception) << "Parallel: throws exception";
}

TEST_F(GeometryTest, RotationMatrix) {
    
    wcl::SMatrix r = getRotationMatrix(90);

    wcl::SMatrix M(4);
    //make it a 4x4 matrix
    M[3][0] = 0;    M[3][1] = 0;
    M[3][2] = 0;    M[0][3] = 0;
    M[1][3] = 0;    M[2][3] = 0;
    M[3][3] = 1;

    M[1][0] = 0;    M[2][0] = 0;
    M[0][1] = 0;    M[2][1] = 0;
    M[0][2] = 0;    M[1][2] = 0;

    float theta = cos( 90 * PI / 180 );
    M[0][0] = theta; M[1][1] = theta ; M[2][2] = theta;

    ASSERT_EQ ( 0, M == r) << "m: " << M;
}

//TEST_F(GeometryTest, VectorRotation45Degrees) {
//
//    wcl::Vector v(0,1,0);
//    wcl::Quaternion q(v);
//
//    wcl::Vector w(0.707,0.707,0);
//
//    wcl::SMatrix rotationMatrix = getRotationMatrix(45);
//    rotationMatrix.cullToZero();
//
//    wcl::Quaternion qrot(rotationMatrix);
//    wcl::Quaternion rotated = qrot.rotate(q);
//    wcl::Vector rotV(rotated.w, rotated.x, rotated.y);
//
//    wcl::Vector output(rotated.w, rotated.x, rotated.y);
//    std::cout << "Input: " << v << std::endl;
//    std::cout << "Rotation matrix: " << std::endl << rotationMatrix << std::endl;
//    std::cout << "Output: " << output << std::endl;
//    std::cout << "Expected: " << w << std::endl;
//    
//    ASSERT_TRUE ( w == output );
//}

TEST_F(GeometryTest, VectorRotation90Degrees) {

    wcl::Vector v(0,1,0);
    wcl::Quaternion q(v);

    wcl::Vector w(1,0,0);
    wcl::Quaternion output(w);

    wcl::SMatrix rotationMatrix = getRotationMatrix(90);
    rotationMatrix.cullToZero();

    wcl::Quaternion qrot(rotationMatrix);
    wcl::Quaternion rotated = qrot.rotate(q);

//    std::cout << "Input: " << v << std::endl;
//    std::cout << "Rotation matrix: " << std::endl << rotationMatrix << std::endl;
//    std::cout << "Output: " << rotated << std::endl;
    
    ASSERT_TRUE ( rotated == output );
}

TEST_F(GeometryTest, intersectLineSegment) {
   
    wcl::LineSegment ls(wcl::Vector(0,0,0), wcl::Vector(2,2,2));
    wcl::LineSegment lr(wcl::Vector(2,2,2), wcl::Vector(0,0,0));

    wcl::LineSegment parallel_a(wcl::Vector(1,0,0), wcl::Vector(4,0,0));
    wcl::LineSegment parallel_b(wcl::Vector(1,2,0), wcl::Vector(4,2,0));

    ASSERT_TRUE(ls.intersect(lr)) << "Line segments " << ls << " and " << lr << " should intersect.";
    ASSERT_FALSE(parallel_a.intersect(parallel_b)) << "Line segments " << parallel_a << " and " << parallel_b << " should not intersect.";
}

TEST_F(GeometryTest, distanceFromPoint) {

    // Test that a point on a line has distance 0 from that line. 
    wcl::Line lp(wcl::Vector(1,1,0), wcl::Vector(0,-1,0));

    wcl::Vector p1 = lp.getPosition();
    wcl::Vector p2 = lp.getPosition() + lp.getDirection(); // One unit along the line. 
    
    ASSERT_EQ(0, lp.distanceFromPoint(p1)) << "Line::pos should have 0 distance from the line.";
    ASSERT_EQ(0, lp.distanceFromPoint(p2)) << "A point (not line::pos) on the line should have 0 distance from the line.";
}

TEST_F(GeometryTest, getPosition) {

    wcl::Vector min(0,0,0);
    wcl::Vector max(1,1,1);
    wcl::Vector a(min);
    wcl::Vector b(min + wcl::Vector(max[0], 0, 0));
    wcl::Vector c(min + wcl::Vector(0, max[1], 0));
    wcl::Vector d(min + wcl::Vector(max[0], max[1], 0));

    wcl::Plane p(a,b,c);

    // I'm not sure if this is always the case, but for the given coordinates it is. 
    ASSERT_FALSE(p.getPosition() == p.getNormal()) << "Check bug in libwcl: Plane::getPosition was returning normal";
    EXPECT_TRUE(p.getPosition() == a);

    ASSERT_TRUE(p.distanceFrom(p.getPosition()) == 0.0);
    ASSERT_FALSE(p.distanceFrom(p.getNormal()) == 0.0);
}
