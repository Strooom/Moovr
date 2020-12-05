#include "CppUnitTest.h"        // this gives you access to the C++ Unit Test framework
#include "logging.h"
#include "machineproperties.h"
#include "motionitemtrajectory.h"
#include "motionitemspeedprofile.h"
#include "motionitemperipherals.h"

uLog theLog;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestEvent {
TEST_CLASS (C01_motionItemTrajectory) {
  public:
    TEST_METHOD (T01_Construction) {
        motionTrajectory theTrajectory;
        //Assert::IsTrue(theBuffer.getBufferLevel() >= theBuffer.minBufferLevel);
    }
};

TEST_CLASS (C02_motionItemSpeedProfile) {
  public:
    TEST_METHOD (T01_Construction) {
        motionSpeedProfile theSpeedProfile;
    }
};

TEST_CLASS (C03_motionItemPeripheral) {
  public:
    TEST_METHOD (T01_Construction) {
        motionPeripherals thePeripherals;
    }
};

TEST_CLASS (C04_motionItem) {
  public:
    TEST_METHOD (T01_Construction) {
        motionItem theMotionItem;
    }
};

TEST_CLASS (C05_motionBuffer) {
  public:
    TEST_METHOD (T01_Construction) {
    }
};
}        // namespace UnitTestEvent
