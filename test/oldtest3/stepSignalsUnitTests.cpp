#include "CppUnitTest.h"        // this gives you access to the C++ Unit Test framework
#include "logging.h"
#include "machineproperties.h"
#include "stepbuffer.h"
#include "stepsignals.h"

extern uLog theLog;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace stepSignalsUnitTests {
TEST_CLASS (C01_StepSignals) {
  public:
    TEST_METHOD (T01_construction_initialization) {
        stepSignals theSignals;
        Assert::AreEqual(0U, theSignals.dirSetup);
        Assert::AreEqual(0U, theSignals.stepRise);
        Assert::AreEqual(0U, theSignals.stepFall);
        Assert::AreEqual(0U, theSignals.lastOutput);
        Assert::AreEqual(0U, theSignals.ticksSinceLastOutput);
    }

    TEST_METHOD (T02_stepForward) {
        stepSignals theSignals;
        theSignals.stepForward(0);
        Assert::IsTrue(theSignals.dirSetup == 0b00);
        Assert::IsTrue(theSignals.stepRise == 0b01);
        Assert::IsTrue(theSignals.stepFall == 0b00);
    }

    TEST_METHOD (T03_stepForward_3Axis) {
        stepSignals theSignals;
        theSignals.stepForward(0);
        theSignals.stepForward(1);
        theSignals.stepForward(2);
        Assert::IsTrue(theSignals.dirSetup == 0b00000000);
        Assert::IsTrue(theSignals.stepRise == 0b00010101);
        Assert::IsTrue(theSignals.stepFall == 0b00000000);
    }

    TEST_METHOD (T04_stepBackward) {
        stepSignals theSignals;
        theSignals.stepBackward(0);
        Assert::IsTrue(theSignals.dirSetup == 0b10);
        Assert::IsTrue(theSignals.stepRise == 0b11);
        Assert::IsTrue(theSignals.stepFall == 0b10);
    }

    TEST_METHOD (T05_stepBackward_3Axis) {
        stepSignals theSignals;
        theSignals.stepBackward(0);
        theSignals.stepBackward(1);
        theSignals.stepBackward(2);
        Assert::IsTrue(theSignals.dirSetup == 0b00101010);
        Assert::IsTrue(theSignals.stepRise == 0b00111111);
        Assert::IsTrue(theSignals.stepFall == 0b00101010);
    }

    TEST_METHOD (T06_next) {
        stepSignals theSignals;
        theSignals.dirSetup = 0x1;
        theSignals.stepRise = 0x2;
        theSignals.stepFall = 0x3;
        theSignals.next();
        Assert::IsTrue(theSignals.dirSetup == 2);
        Assert::IsTrue(theSignals.stepRise == 3);
        Assert::IsTrue(theSignals.stepFall == 3);
        theSignals.next();
        Assert::IsTrue(theSignals.dirSetup == 3);
        Assert::IsTrue(theSignals.stepRise == 3);
        Assert::IsTrue(theSignals.stepFall == 3);
    }

    TEST_METHOD (T07_isModified) {
        stepSignals theSignals;
        Assert::IsFalse(theSignals.isModified());
        theSignals.stepForward(0);
        Assert::IsFalse(theSignals.isModified());
        theSignals.next();
        Assert::IsTrue(theSignals.isModified());
    }

    TEST_METHOD (T08_output) {
        stepSignals theSignals;
        step aStep = theSignals.output();
        Assert::AreEqual(0U, aStep.signals);
        Assert::AreEqual(0U, aStep.timeBefore);
        theSignals.next();
        aStep = theSignals.output();
        Assert::AreEqual(0U, aStep.signals);
        Assert::AreEqual(1U, aStep.timeBefore);
        theSignals.stepForward(0);
        theSignals.next();
        aStep = theSignals.output();
        Assert::AreEqual(1U, aStep.signals);
        Assert::AreEqual(1U, aStep.timeBefore);
    }
};
}        // namespace stepSignalsUnitTests
