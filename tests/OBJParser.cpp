#include <gtest/gtest.h>

#include <wcl/parsers/OBJParser.cpp>


// The fixture for testing wcl::BoundingBox. 
class OBJParserTest : public ::testing::Test {
};


TEST(OBJParserTest, FileNotFound) {
    OBJParser p;
    EXPECT_THROW(p.parse("filedoesnotexist.obj"), WCLException);
}
    


