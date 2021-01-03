#include "CppUnitTest.h"
#include "mainctrl.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestsMainCtrl {
TEST_CLASS (C01_mainCtrl) {
  public:
    TEST_METHOD (T01_constructor) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        HostInterfaceUart theHostInterface;
        stepBuffer theStepBuffer;
        mainController theCtrl(theMachineProperties, theEventBuffer, theHostInterface, theStepBuffer);
        Assert::IsTrue(theCtrl.theMainState == mainState::Ready);
        Assert::IsTrue(theCtrl.theHomingState == homingState::Lost);
        Assert::IsTrue(theCtrl.theProbingState == probingState::Lost);
    }
    TEST_METHOD (T02_initialization) {
    }
};
}        // namespace UnitTestsMainCtrl