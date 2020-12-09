#include "CppUnitTest.h"        // this gives you access to the C++ Unit Test framework
#include "logging.h"
#include "machineproperties.h"
#include "motion.h"
#include "motionbuffer.h"
#include "motionctrl.h"
#include "motionperipherals.h"
#include "motionspeedprofile.h"
#include "motiontrajectory.h"

uLog theLog;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestMotion {
TEST_CLASS (C01_motionTrajectory) {
  public:
    TEST_METHOD (T01_Construction) {
        motionTrajectory theTrajectory;
    }
};

TEST_CLASS (C02_motionSpeedProfile) {
  public:
    TEST_METHOD (T01_Construction) {
        motionSpeedProfile theSpeedProfile;
    }
};

TEST_CLASS (C03_motionPeripheral) {
  public:
    TEST_METHOD (T01_Construction) {
        motionPeripherals thePeripherals;
    }
};

TEST_CLASS (C04_motion) {
  public:
    TEST_METHOD (T01_Construction) {
        motion theMotion;
    }
};

TEST_CLASS (C05_motionBuffer) {
  public:
    TEST_METHOD (T01_Construction) {
        motionBuffer theMotionBuffer;
        Assert::IsTrue(theMotionBuffer.isEmpty(), L"001");
        uint32_t newItemIndex = theMotionBuffer.push();
        Assert::IsTrue(newItemIndex == 0, L"002");
        Assert::IsFalse(theMotionBuffer.isEmpty(), L"003");
        Assert::IsTrue(theMotionBuffer.bufferLevel == 1, L"004");
    }
};

TEST_CLASS (C06_motionCtrl) {
  public:
    TEST_METHOD (T01_Construction) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }
};

}        // namespace UnitTestMotion
