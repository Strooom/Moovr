
#include <unity.h>
#include "motionctrl.h"
#include "machineproperties.h"        // motionControl relies on an external dependency : there must be a global instance of machineProperties where it can read settings/properties
#include "eventbuffer.h"              // motionControl relies on an external dependency : there must be a global instance of eventBuffer where it can notify/push events that occured
#include <math.h>

// ----------------------------------
// --- to be tested API           ---
// ----------------------------------

// void append(simplifiedMotion &aSimpleMotion);
// void start();
// void hold();
// void resume();
// void stop();

// void optimize();

// motionState getState() const;
// motionStrategy getMotionStrategy() const;
// overrides getOverrides() const;
// bool isRunning() const;

// step calcNextStepperMotorSignals();

// ----------------------------------
// --- globals                    ---
// ----------------------------------

machineProperties theMachineProperties;        //
eventBuffer theEventBuffer;                    //

void setUp(void) {
    theMachineProperties.motors.aMax[0] = 1.0f;
    theMachineProperties.motors.dMax[0] = -1.0f;
    theMachineProperties.motors.vMax[0] = 1.0f;
    theMachineProperties.motors.aMax[1] = 1.0f;
    theMachineProperties.motors.dMax[1] = -1.0f;
    theMachineProperties.motors.vMax[1] = 1.0f;
    theMachineProperties.motors.aMax[2] = 1.0f;
    theMachineProperties.motors.dMax[2] = -1.0f;
    theMachineProperties.motors.vMax[2] = 1.0f;
}
void tearDown(void) {}        // after test

void initialize() {
    motionCtrl theMotionCtrl;
    TEST_ASSERT_FALSE(theMotionCtrl.isRunning());
    TEST_ASSERT_EQUAL(motionState::stopped, theMotionCtrl.getState());
    TEST_ASSERT_EQUAL(1.0f, theMotionCtrl.theOverrides.feedOverride);
    TEST_ASSERT_EQUAL(1.0f, theMotionCtrl.theOverrides.spindleOverride);
    TEST_ASSERT_FALSE(theMotionCtrl.isOptimal);
    for (uint32_t anAxis = 0; anAxis < nmbrAxis; anAxis++) {
        TEST_ASSERT_EQUAL(0, theMotionCtrl.currentPositionInSteps[anAxis]);
        TEST_ASSERT_EQUAL(0.0F, theMotionCtrl.nextPositionInMm[anAxis]);
    }
}

void test_calcNextPositionInMm() {
    motionCtrl theMotionCtrl;
    simplifiedMotion aSimpleMotion;
    aSimpleMotion.setForTest(0U);
    theMotionCtrl.append(aSimpleMotion);
    theMotionCtrl.start(); // now start moving...

    // initial position should be all zero..

    for (uint32_t anAxis = 0; anAxis < nmbrAxis; anAxis++) {
        theMotionCtrl.positionInMm(anAxis, 0.0F, theMotionCtrl.theMotionBuffer.getHead().trajectory);
        TEST_ASSERT_EQUAL(0.0F, theMotionCtrl.nextPositionInMm[anAxis]);
    }

    for (uint32_t anAxis = 0; anAxis < nmbrAxis; anAxis++) {
        theMotionCtrl.positionInMm(anAxis, 0.5F, theMotionCtrl.theMotionBuffer.getHead().trajectory);
    }
    TEST_ASSERT_EQUAL(0.0F, theMotionCtrl.nextPositionInMm[2]);
    TEST_ASSERT_EQUAL(0.0F, theMotionCtrl.nextPositionInMm[1]);
    TEST_ASSERT_EQUAL(0.5F, theMotionCtrl.nextPositionInMm[0]);

    for (uint32_t anAxis = 0; anAxis < nmbrAxis; anAxis++) {
        theMotionCtrl.positionInMm(anAxis, 1.0F, theMotionCtrl.theMotionBuffer.getHead().trajectory);
    }
    TEST_ASSERT_EQUAL(0.0F, theMotionCtrl.nextPositionInMm[2]);
    TEST_ASSERT_EQUAL(0.0F, theMotionCtrl.nextPositionInMm[1]);
    TEST_ASSERT_EQUAL(1.0F, theMotionCtrl.nextPositionInMm[0]);
}


void test_optimize() {
    TEST_FAIL_MESSAGE("missing test");
}

void test_optimizePair() {
    TEST_FAIL_MESSAGE("missing test");
}

void test_vJunction() {
    TEST_FAIL_MESSAGE("missing test");
}

void test_calcNextPositionInSteps() {
    TEST_FAIL_MESSAGE("missing test");
}

void nextStep_notRunning() {
    step aStep;
    motionCtrl theMotionCtrl;

    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(0U, aStep.signals);                                  // we should get default stepSignals..
    TEST_ASSERT_EQUAL(maxTicksSinceLastOutput, aStep.timeBefore);          //
    TEST_ASSERT_EQUAL(0U, theMotionCtrl.theSampleTime.sampleIndex);        // and the sampleTime should not proceed
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialize);
    RUN_TEST(test_calcNextPositionInMm);

    RUN_TEST(test_optimize);
    RUN_TEST(test_optimizePair);
    RUN_TEST(test_vJunction);
    RUN_TEST(test_calcNextPositionInSteps);
    RUN_TEST(nextStep_notRunning);
    UNITY_END();
}