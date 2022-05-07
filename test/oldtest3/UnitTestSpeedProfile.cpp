#include "CppUnitTest.h"
#include "motionspeedprofile.h"
#include "gcode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestsSpeedProfile {
TEST_CLASS (C01_SpeedProfile) {
  public:
    TEST_METHOD (T01_constructor) {
        motionSpeedProfile theSpeedPofile;
        Assert::AreEqual(theSpeedPofile.duration, 0.0F);
    }
    TEST_METHOD (T02_setSpeed) {
        gCode theParser;
        gCodeParserResult theResult;
        motionSpeedProfile theSpeedPofile;
        theParser.initialize();
        theParser.getBlock("G1 X80 F1200");
        theParser.parseBlock(theResult);
        theSpeedPofile.setSpeed(theResult);
        Assert::AreEqual(theSpeedPofile.vFeed, 20.0F);
    }
    TEST_METHOD (T03_setDuration) {
        gCode theParser;
        gCodeParserResult theResult;
        motionSpeedProfile theSpeedPofile;
        theParser.initialize();
        theParser.getBlock("G4 P10");
        theParser.parseBlock(theResult);
        theSpeedPofile.setDuration(theResult);
        Assert::AreEqual(theSpeedPofile.duration, 10.0F);
    }
};
}        // namespace UnitTestsGcode