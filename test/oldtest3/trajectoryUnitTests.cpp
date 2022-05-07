#include "CppUnitTest.h"
#include "motiontrajectory.h"
#include "gcode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trajectoryUnitTests {
TEST_CLASS (C01_trajectory) {
  public:
    TEST_METHOD (T01_constructor) {
        motionTrajectory theTrajectory; 
        Assert::AreEqual(theTrajectory.length, 0.0F);
    }
    TEST_METHOD (T02_set) {
        gCode theParser;
        gCodeParserResult theResult;
        motionTrajectory theTrajectory;
        theParser.initialize();
        theParser.getBlock("G1 X80 F1200");
        theParser.parseBlock(theResult);
        theTrajectory.set(theResult);
        Assert::AreEqual(theTrajectory.length, 80.0F);
    }
};
}        // namespace UnitTestsGcode