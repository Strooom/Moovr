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

    TEST_METHOD (T002_getNextStep) {
        Logger::WriteMessage("motionBuffer empty ->  : output is default");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        Assert::IsTrue(theMotionCtrl.theMotionBuffer.isEmpty());
        step aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T003_getNextStep) {
        Logger::WriteMessage("motionBuffer NOT empty, state = ready or stopped ->  : output is default");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionCtrl.theState = motionState::ready;
        (void)theMotionCtrl.theMotionBuffer.push();
        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
        theMotionCtrl.theState = motionState::stopped;
        aStep                        = theMotionCtrl.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T004_getNextStep) {
        Logger::WriteMessage("motionBuffer NOT empty, state = running or stopping, slow movement ->  : output is default");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionCtrl.theState = motionState::running;
        (void)theMotionCtrl.theMotionBuffer.push();
        motion* aMotion = theMotionCtrl.theMotionBuffer.current();
        aMotion->setForTest(0U);

        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
        theMotionCtrl.theState = motionState::stopping;
        aStep                        = theMotionCtrl.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T005_getNextStep) {
        Logger::WriteMessage("motionBuffer NOT empty, state = running or stopping, fast movement ->  : output is steps");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionCtrl.theState = motionState::running;
        (void)theMotionCtrl.theMotionBuffer.push();
        motion* aMotion = theMotionCtrl.theMotionBuffer.current();
        aMotion->setForTest(1U);
        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(aStep.timeBefore <= 35);
        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(aStep.timeBefore == 1);
        theMotionCtrl.theState = motionState::stopping;
        aStep                        = theMotionCtrl.nextStep();
        Assert::IsTrue(aStep.timeBefore <= 35);
        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(aStep.timeBefore == 1);
    }

    TEST_METHOD (T006_MotionComplete) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionCtrl.theState = motionState::running;
        (void)theMotionCtrl.theMotionBuffer.push();
        theMotionCtrl.theMotionBuffer.motionBuffer[0].setForTest(1U);
        (void)theMotionCtrl.theMotionBuffer.push();
        theMotionCtrl.theMotionBuffer.motionBuffer[1].setForTest(1U);

        Assert::IsFalse(theEventBuffer.hasEvents());
        for (int i = 0; i < 80; i++) {
            aStep = theMotionCtrl.nextStep();
        }
        Assert::IsFalse(theEventBuffer.hasEvents());
        for (int i = 0; i < 40; i++) {
            aStep = theMotionCtrl.nextStep();
        }
        Assert::IsTrue(theEventBuffer.hasEvents());
    }

    TEST_METHOD (T005_Construction) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }

    TEST_METHOD (T006_vJunction) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }

    TEST_METHOD (T007_OptimizePair) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }
};

TEST_CLASS (C01_motionCtrl) {
  public:


    TEST_METHOD (T03_nextStep_Running_emptyBuffer) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theCtrl.theState = motionState::running;
        aStep            = theCtrl.nextStep();
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
        motionCtrl theCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);

        uint32_t index   = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(3U);
        theCtrl.theState = motionState::running;

        aStep = theCtrl.nextStep();
        Assert::AreEqual(0U, theCtrl.theSampleTime.sampleIndex);
    }

    TEST_METHOD (T04_nextStep_Running_T01) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);

        uint32_t index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(3U);
        index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(3U);
        theCtrl.theState = motionState::running;

        aStep = theCtrl.nextStep();
        Assert::AreEqual(0U, theCtrl.theSampleTime.sampleIndex);
    }



    TEST_METHOD (T05_nextStep_Running_T02) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);

        uint32_t index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(3U);
        index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(2U);
        index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(3U);
        theCtrl.theState = motionState::running;

        aStep = theCtrl.nextStep();
        Assert::AreEqual(0U, theCtrl.theSampleTime.sampleIndex);
    }


    TEST_METHOD (T06_nextStep_Running_T10) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);

        uint32_t index = theCtrl.theMotionBuffer.push();
        theCtrl.theMotionBuffer.motionBuffer[index].setForTest(4U);

        theCtrl.theState = motionState::running;

        aStep = theCtrl.nextStep();
        Assert::AreEqual(0U, theCtrl.theSampleTime.sampleIndex);
    }



    TEST_METHOD (T004_getNextStep) {
        Logger::WriteMessage("motionBuffer NOT empty, state = running or stopping, slow movement ->  : output is default");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionCtrl.theState = motionState::running;
        (void)theMotionCtrl.theMotionBuffer.push();
        motion* aMotion = theMotionCtrl.theMotionBuffer.current();
        aMotion->setForTest(0U);

        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
        theMotionCtrl.theState = motionState::stopping;
        aStep                        = theMotionCtrl.nextStep();
        Assert::IsTrue(maxTicksSinceLastOutput == aStep.timeBefore);
    }

    TEST_METHOD (T005_getNextStep) {
        Logger::WriteMessage("motionBuffer NOT empty, state = running or stopping, fast movement ->  : output is steps");
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionCtrl.theState = motionState::running;
        (void)theMotionCtrl.theMotionBuffer.push();
        motion* aMotion = theMotionCtrl.theMotionBuffer.current();
        aMotion->setForTest(1U);
        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(aStep.timeBefore <= 35);
        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(aStep.timeBefore == 1);
        theMotionCtrl.theState = motionState::stopping;
        aStep                        = theMotionCtrl.nextStep();
        Assert::IsTrue(aStep.timeBefore <= 35);
        aStep = theMotionCtrl.nextStep();
        Assert::IsTrue(aStep.timeBefore == 1);
    }

    TEST_METHOD (T006_MotionComplete) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        step aStep;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMotionCtrl.theState = motionState::running;
        (void)theMotionCtrl.theMotionBuffer.push();
        theMotionCtrl.theMotionBuffer.motionBuffer[0].setForTest(1U);
        (void)theMotionCtrl.theMotionBuffer.push();
        theMotionCtrl.theMotionBuffer.motionBuffer[1].setForTest(1U);

        Assert::IsFalse(theEventBuffer.hasEvents());
        for (int i = 0; i < 80; i++) {
            aStep = theMotionCtrl.nextStep();
        }
        Assert::IsFalse(theEventBuffer.hasEvents());
        for (int i = 0; i < 40; i++) {
            aStep = theMotionCtrl.nextStep();
        }
        Assert::IsTrue(theEventBuffer.hasEvents());
    }


    TEST_METHOD (T006_vJunction) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }

    TEST_METHOD (T007_OptimizePair) {
        eventBuffer theEventBuffer;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theMotionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
    }
};


