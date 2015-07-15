#include <gtest/gtest.h>

#include <wcl/parser/OBJParser.cpp>


// The fixture for testing wcl::BoundingBox. 
class OBJParserTest : public ::testing::Test {
};


TEST(OBJParserTest, FileNotFound) {
    wcl::OBJParser p;
    EXPECT_THROW(p.parse("filedoesnotexist.obj"), wcl::ParserException);
}
    


