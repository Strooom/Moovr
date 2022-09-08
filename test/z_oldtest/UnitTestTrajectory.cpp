#include "CppUnitTest.h"
#include "motiontrajectory.h"
#include "gcode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestsTrajectory {
TEST_CLASS (C01_Trajectory) {
  public:
    TEST_METHOD (T01_constructor) {
        motionTrajectory theTrajectory; 
        Assert::AreEqual(theTrajectory.length, 0.0F);
    }
    TEST_METHOD (T02_set) {
        gCode theInterpreter;
        gCodeParserResult theResult;
        motionTrajectory theTrajectory;
        theInterpreter.initialize();
        theInterpreter.getBlock("G1 X80 F1200");
        theInterpreter.interpreteBlock(theResult);
        theTrajectory.set(theResult);
        Assert::AreEqual(theTrajectory.length, 80.0F);
    }
};
}        // namespace UnitTestsGcode