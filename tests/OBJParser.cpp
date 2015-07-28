
#include <wcl/parser/OBJParser.h>
#include <sstream>

#include <gtest/gtest.h>

// The fixture for testing wcl::BoundingBox. 
class OBJParserTest : public ::testing::Test {
};


// parse should throw an exception if the file could not be opened.
TEST(OBJParserTest, FileNotFound) {
    wcl::OBJParser p;
    EXPECT_THROW(p.parse("filedoesnotexist.obj"), wcl::ParserException);
}
    

// Make sure OBJParser can 
TEST(OBJParserTest, GetLineType) {
    wcl::OBJParser p;

    EXPECT_EQ(wcl::OBJParser::FACE, p.getLineType("f"));
    EXPECT_EQ(wcl::OBJParser::GROUP, p.getLineType("g"));
    EXPECT_EQ(wcl::OBJParser::MTLLIB, p.getLineType("mtllib"));
    EXPECT_EQ(wcl::OBJParser::NORMAL, p.getLineType("vn"));
    EXPECT_EQ(wcl::OBJParser::OBJECT, p.getLineType("o"));
    EXPECT_EQ(wcl::OBJParser::TEXCOORD, p.getLineType("vt"));
    EXPECT_EQ(wcl::OBJParser::USEMTL, p.getLineType("usemtl"));
    EXPECT_EQ(wcl::OBJParser::VERTEX, p.getLineType("v"));
}


// make sure we can read vectors
TEST(OBJParserTest, ParseVectorSimple) {
    std::istringstream tokens("4 5 6");
    wcl::OBJParser p;

    wcl::Vector v1 = p.parseVector(tokens);
    EXPECT_EQ(v1[0], 4);
    EXPECT_EQ(v1[1], 5);
    EXPECT_EQ(v1[2], 6);
}

TEST(OBJParserTest, ParseVectorFloat) {
    std::istringstream tokens("4.3542 5.12 6.2359");
    wcl::OBJParser p;

    wcl::Vector v1 = p.parseVector(tokens);
    EXPECT_EQ(v1[0], 4.3542);
    EXPECT_EQ(v1[1], 5.12);
    EXPECT_EQ(v1[2], 6.2359);
}


// make sure we can add vertices
TEST(OBJParserTest, AddVertex) {
    wcl::OBJGeometry obj;
    wcl::OBJParser p;
    p.parseLine("v 12 54 3", obj);
    EXPECT_EQ(obj.points.size(), 1);
    EXPECT_EQ((obj.points[0])[0], 12);
    EXPECT_EQ((obj.points[0])[1], 54);
    EXPECT_EQ((obj.points[0])[2], 3);

    p.parseLine("v 1 2 3", obj);
    EXPECT_EQ(obj.points.size(), 2);
    EXPECT_EQ((obj.points[0])[0], 12);
    EXPECT_EQ((obj.points[0])[1], 54);
    EXPECT_EQ((obj.points[0])[2], 3);

    EXPECT_EQ((obj.points[1])[0], 1);
    EXPECT_EQ((obj.points[1])[1], 2);
    EXPECT_EQ((obj.points[1])[2], 3);

    // make sure we didn't also add anything else
    EXPECT_EQ(obj.normals.size(), 0);
    EXPECT_EQ(obj.texcoords.size(), 0);
}

// make sure we can add normals 
TEST(OBJParserTest, AddNormal) {
    wcl::OBJGeometry obj;
    wcl::OBJParser p;
    p.parseLine("vn 2 34 32", obj);
    EXPECT_EQ(obj.normals.size(), 1);
    EXPECT_EQ((obj.normals[0])[0], 2);
    EXPECT_EQ((obj.normals[0])[1], 34);
    EXPECT_EQ((obj.normals[0])[2], 32);

    p.parseLine("vn 1 2 3", obj);
    EXPECT_EQ(obj.normals.size(), 2);
    EXPECT_EQ((obj.normals[0])[0], 2);
    EXPECT_EQ((obj.normals[0])[1], 34);
    EXPECT_EQ((obj.normals[0])[2], 32);

    EXPECT_EQ((obj.normals[1])[0], 1);
    EXPECT_EQ((obj.normals[1])[1], 2);
    EXPECT_EQ((obj.normals[1])[2], 3);

    // make sure we didn't also add anything else
    EXPECT_EQ(obj.points.size(), 0);
    EXPECT_EQ(obj.texcoords.size(), 0);
}

// make sure we can add texture coordinates 
TEST(OBJParserTest, AddTexCoord) {
    wcl::OBJGeometry obj;
    wcl::OBJParser p;
    p.parseLine("vt 23 3 393", obj);
    EXPECT_EQ(obj.texcoords.size(), 1);
    EXPECT_EQ((obj.texcoords[0])[0], 23);
    EXPECT_EQ((obj.texcoords[0])[1], 3);
    EXPECT_EQ((obj.texcoords[0])[2], 393);

    p.parseLine("vt 1 2 3", obj);
    EXPECT_EQ(obj.texcoords.size(), 2);
    EXPECT_EQ((obj.texcoords[0])[0], 23);
    EXPECT_EQ((obj.texcoords[0])[1], 3);
    EXPECT_EQ((obj.texcoords[0])[2], 393);

    EXPECT_EQ((obj.texcoords[1])[0], 1);
    EXPECT_EQ((obj.texcoords[1])[1], 2);
    EXPECT_EQ((obj.texcoords[1])[2], 3);

    // make sure we didn't also add anything else
    EXPECT_EQ(obj.points.size(), 0);
    EXPECT_EQ(obj.normals.size(), 0);
}
