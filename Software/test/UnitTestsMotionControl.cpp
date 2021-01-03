#include "CppUnitTest.h"        // this gives you access to the C++ Unit Test framework
#include "gcode.h"
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

namespace UnitTestMotionCtrl {

TEST_CLASS (C06_motionCtrl) {
  public:
    TEST_METHOD (T01_Construction) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }

    TEST_METHOD (T002_getNextStep) {
        Logger::WriteMessage("motionBuffer empty ->  : output is default");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        Assert::IsTrue(theMotionController.theMotionBuffer.isEmpty());
        step aStep = theMotionController.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T003_getNextStep) {
        Logger::WriteMessage("motionBuffer NOT empty, state = ready or stopped ->  : output is default");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionController.theState = motionState::ready;
        (void)theMotionController.theMotionBuffer.push();
        aStep = theMotionController.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
        theMotionController.theState = motionState::stopped;
        aStep                        = theMotionController.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T004_getNextStep) {
        Logger::WriteMessage("motionBuffer NOT empty, state = running or stopping, slow movement ->  : output is default");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionController.theState = motionState::running;
        (void)theMotionController.theMotionBuffer.push();
        motion* aMotion = theMotionController.theMotionBuffer.current();
        aMotion->setForTest(0U);

        aStep = theMotionController.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
        theMotionController.theState = motionState::stopping;
        aStep                        = theMotionController.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T005_getNextStep) {
        Logger::WriteMessage("motionBuffer NOT empty, state = running or stopping, fast movement ->  : output is steps");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionController.theState = motionState::running;
        (void)theMotionController.theMotionBuffer.push();
        motion* aMotion = theMotionController.theMotionBuffer.current();
        aMotion->setForTest(1U);
        aStep = theMotionController.nextStep();
        Assert::IsTrue(aStep.timeBefore <= 35);
        aStep = theMotionController.nextStep();
        Assert::IsTrue(aStep.timeBefore == 1);
        theMotionController.theState = motionState::stopping;
        aStep                        = theMotionController.nextStep();
        Assert::IsTrue(aStep.timeBefore <= 35);
        aStep = theMotionController.nextStep();
        Assert::IsTrue(aStep.timeBefore == 1);
    }

    TEST_METHOD (T006_MotionComplete) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionController.theState = motionState::running;
        (void)theMotionController.theMotionBuffer.push();
        theMotionController.theMotionBuffer.motionBuffer[0].setForTest(1U);
        (void)theMotionController.theMotionBuffer.push();
        theMotionController.theMotionBuffer.motionBuffer[1].setForTest(1U);

        Assert::IsFalse(theEventBuffer.hasEvents());
        for (int i = 0; i < 80; i++) {
            aStep = theMotionController.nextStep();
        }
        Assert::IsFalse(theEventBuffer.hasEvents());
        for (int i = 0; i < 40; i++) {
            aStep = theMotionController.nextStep();
        }
        Assert::IsTrue(theEventBuffer.hasEvents());
    }

    TEST_METHOD (T005_Construction) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }

    TEST_METHOD (T006_vJunction) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }

    TEST_METHOD (T007_OptimizePair) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }
};
}        // namespace UnitTestMotionCtrl
