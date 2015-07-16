#include <gtest/gtest.h>

#include <wcl/parser/OBJParser.cpp>


// The fixture for testing wcl::BoundingBox. 
class OBJParserTest : public ::testing::Test {
};


// parse should throw an exception if the file could not be opened.
TEST(OBJParserTest, FileNotFound) {
    wcl::OBJParser p;
    EXPECT_THROW(p.parse("filedoesnotexist.obj"), wcl::ParserException);
}
    

TEST(OBJParserTest, GetLineType) {
    wcl::OBJParser p;

    EXPECT_EQ(wcl::OBJParser::VERTEX, p.getLineType("v"));
    EXPECT_EQ(wcl::OBJParser::NORMAL, p.getLineType("n"));
    EXPECT_EQ(wcl::OBJParser::TEX_COORD, p.getLineType("vt"));
    EXPECT_EQ(wcl::OBJParser::FACE, p.getLineType("f"));
    EXPECT_EQ(wcl::OBJParser::USE_MTL, p.getLineType("usemtl"));
}


