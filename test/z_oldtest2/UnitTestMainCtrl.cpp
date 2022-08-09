#include "CppUnitTest.h"
#include "mainctrl.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestsMainCtrl {
TEST_CLASS (C01_mainCtrl) {
  public:
    TEST_METHOD (T01_constructor) {
        inputs theHardwareInputs;
        hostInterfaceUart theHostInterface;
        stepBuffer theStepBuffer;

        mainController theCtrl(theHardwareInputs, theHostInterface, theStepBuffer);
        Assert::IsTrue(theCtrl.mainState == mainStates::ready);
        Assert::IsTrue(theCtrl.homingState == homingStates::lost);
        Assert::IsTrue(theCtrl.probingState == probingStates::lost);
    }
    TEST_METHOD (T02_initialization) {
    }
};
}        // namespace UnitTestsMainCtrl