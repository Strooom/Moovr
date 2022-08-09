#include "CppUnitTest.h"        // this gives you access to the C++ Unit Test framework

#include "logging.h"
#include "motionctrl.h"
#include "stepperoutputs_hal.h"

extern uLog theLog;
extern stepperMotorOutputs theStepperMotorOutputs;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace motionCtrlUnitTest {

TEST_CLASS (C01_motionCtrl) {
  public:
    TEST_METHOD (T01_Construction) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);

        Assert::IsTrue(motionState::stopped == theCtrl.state);
        Assert::IsFalse(theCtrl.isOptimal);
        for (uint8_t i = 0; i < (uint8_t)axis::nmbrAxis; i++) {
            Assert::AreEqual(0, theCtrl.currentPositionInSteps[i]);
            Assert::AreEqual(0.0F, theCtrl.nextPositionInMm[i]);
        }
    }

    TEST_METHOD (T02_Reset) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);
        for (uint8_t i = 0; i < (uint8_t)axis::nmbrAxis; i++) {
            Assert::AreEqual(0, theCtrl.currentPositionInSteps[i]);
            Assert::AreEqual(0.0F, theCtrl.nextPositionInMm[i]);
        }
        Assert::AreEqual(0U, theCtrl.theMotionBuffer.getLevel());
    }

    TEST_METHOD (T03_nextStep) {
        // motionBuffer empty ->  : output is default
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);
        Assert::IsTrue(theMotionCtrl.theMotionBuffer.isEmpty());
        step aStep = theMotionCtrl.nextStep();
        Assert::AreEqual(0U, aStep.signals);                                  // we should get default stepSignals..
        Assert::AreEqual(maxTicksSinceLastOutput, aStep.timeBefore);          // default timer reload times
        Assert::AreEqual(0U, theMotionCtrl.theSampleTime.sampleIndex);        // and the sampleTime should not proceed
    }

    TEST_METHOD (T04_nextStep) {
        // motionBuffer NOT empty but motionState = stopped ->  : output is default
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);
        Assert::IsTrue(theMotionCtrl.state == motionState::stopped);
        (void)theMotionCtrl.theMotionBuffer.push();
        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T05_nextStep) {
        // motionBuffer NOT empty, state = running, SLOW movement ->  : output is default
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);
        theMotionCtrl.state = motionState::running;
        (void)theMotionCtrl.theMotionBuffer.push();
        theMotionCtrl.theMotionBuffer.motionBuffer[0].setForTest(0U);        // testMotion #0

        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T06_nextStep) {
        // motionBuffer NOT empty, state = running or stopping, fast movement ->  : output is steps
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);
        theMotionCtrl.state = motionState::running;
        (void)theMotionCtrl.theMotionBuffer.push();
        theMotionCtrl.theMotionBuffer.motionBuffer[0].setForTest(1U);        // testMotion #1

        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(aStep.timeBefore <= 35);
        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(aStep.timeBefore == 1);
    }

    TEST_METHOD (T07_motionComplete) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);
        theMotionCtrl.state = motionState::running;
        (void)theMotionCtrl.theMotionBuffer.push();
        theMotionCtrl.theMotionBuffer.motionBuffer[0].setForTest(1U);

        Assert::IsFalse(theEventBuffer.hasEvents());
        for (int i = 0; i < 80; i++) {
            aStep = theMotionCtrl.nextStep();
        }
        Assert::IsFalse(theEventBuffer.hasEvents());
        for (int i = 0; i < 40; i++) {
            aStep = theMotionCtrl.nextStep();
        }
        Assert::IsTrue(event::motionCompleted == theEventBuffer.popEvent());
        Assert::IsTrue(event::allMotionsCompleted == theEventBuffer.popEvent());
        Assert::IsTrue(event::motionStopped == theEventBuffer.popEvent());
        Assert::IsFalse(theEventBuffer.hasEvents());
    }

    TEST_METHOD (T03_nextStep_Running_emptyBuffer) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);
        theCtrl.state = motionState::running;
        aStep         = theCtrl.nextStep();
        Assert::AreEqual(0U, aStep.signals);        // we should get default stepSignals..
        Assert::AreEqual(maxTicksSinceLastOutput, aStep.timeBefore);
        Assert::AreEqual(1U, theCtrl.theSampleTime.sampleIndex);        // and the sampleTime should only proceed once, as the empty buffer issues a stop
    }

    TEST_METHOD (T04_nextStep_Running_T00) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);

        uint32_t index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(3U);
        theCtrl.state = motionState::running;

        aStep = theCtrl.nextStep();
        Assert::AreEqual(0U, theCtrl.theSampleTime.sampleIndex);
    }

    TEST_METHOD (T04_nextStep_Running_T01) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);

        uint32_t index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(3U);
        index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(3U);
        theCtrl.state = motionState::running;

        aStep = theCtrl.nextStep();
        Assert::AreEqual(0U, theCtrl.theSampleTime.sampleIndex);
    }

    TEST_METHOD (T05_nextStep_Running_T02) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);

        uint32_t index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(3U);
        index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(2U);
        index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(3U);
        theCtrl.state = motionState::running;

        aStep = theCtrl.nextStep();
        Assert::AreEqual(0U, theCtrl.theSampleTime.sampleIndex);
    }

    TEST_METHOD (T06_nextStep_Running_T10) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theCtrl(theStepBuffer, theEventBuffer, theMachineProperties, theOverrides);

        uint32_t index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(4U);

        theCtrl.state = motionState::running;

        aStep = theCtrl.nextStep();
        Assert::AreEqual(0U, theCtrl.theSampleTime.sampleIndex);
    }
};
}        // namespace motionCtrlUnitTest
