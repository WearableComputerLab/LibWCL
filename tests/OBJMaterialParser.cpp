
#include <wcl/parser/OBJMaterialParser.h>
#include <sstream>

#include <gtest/gtest.h>

// The fixture for testing wcl::BoundingBox. 
class OBJParserTest : public ::testing::Test {
};


// parse should throw an exception if the file could not be opened.
TEST(OBJMaterialParserTest, FileNotFound) {
    wcl::OBJMaterialParser p;
    EXPECT_THROW(p.parse("filedoesnotexist.mtl"), wcl::ParserException);
}

TEST(OBJMaterialParserTest, GetLineType) {
    wcl::OBJMaterialParser p;

    EXPECT_EQ(wcl::OBJMaterialParser::NEWMTL, p.getLineType("newmtl"));
    EXPECT_EQ(wcl::OBJMaterialParser::KA, p.getLineType("Ka"));
    EXPECT_EQ(wcl::OBJMaterialParser::KD, p.getLineType("Kd"));
    EXPECT_EQ(wcl::OBJMaterialParser::KS, p.getLineType("Ks"));
    EXPECT_EQ(wcl::OBJMaterialParser::NS, p.getLineType("Ns"));

    EXPECT_EQ(wcl::OBJMaterialParser::TR, p.getLineType("Tr"));
    EXPECT_EQ(wcl::OBJMaterialParser::TR, p.getLineType("d"));
    EXPECT_EQ(wcl::OBJMaterialParser::ILLUM, p.getLineType("illum"));

    EXPECT_EQ(wcl::OBJMaterialParser::MAP_KA, p.getLineType("map_Ka"));
    EXPECT_EQ(wcl::OBJMaterialParser::MAP_KD, p.getLineType("map_Kd"));
    EXPECT_EQ(wcl::OBJMaterialParser::MAP_KS, p.getLineType("map_Ks"));
    EXPECT_EQ(wcl::OBJMaterialParser::MAP_NS, p.getLineType("map_Ns"));
    EXPECT_EQ(wcl::OBJMaterialParser::MAP_D, p.getLineType("map_d"));

    EXPECT_EQ(wcl::OBJMaterialParser::MAP_BUMP, p.getLineType("map_bump"));
    EXPECT_EQ(wcl::OBJMaterialParser::MAP_BUMP, p.getLineType("bump"));
    EXPECT_EQ(wcl::OBJMaterialParser::MAP_DISP, p.getLineType("disp"));
    EXPECT_EQ(wcl::OBJMaterialParser::MAP_STENCIL, p.getLineType("decal"));
}


TEST(OBJMaterialParserTest, NewMaterial) {
    wcl::OBJMaterialParser p;
}
