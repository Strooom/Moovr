
#include <unity.h>
#include "motionctrl.h"
#include "machineproperties.h"        // motionControl relies on an external dependency : there must be a global instance of machineProperties where it can read settings/properties
#include "eventbuffer.h"              // motionControl relies on an external dependency : there must be a global instance of eventBuffer where it can notify/push events that occured
#include <math.h>
#include <limits>

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
    theMachineProperties.setForTest(1U);
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
        TEST_ASSERT_EQUAL(0, theMotionCtrl.machinePositionInSteps[anAxis]);
    }
}

void nextStep_notRunning() {
    step aStep;
    motionCtrl theMotionCtrl;
    theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(maxTicksSinceLastOutput);

    // empty motionBuffer and motionState == stopped (= default), we should get default stepSignals, and the sampleTime should not proceed
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(0U, aStep.signals);                                  // we should get default stepSignals..
    TEST_ASSERT_EQUAL(maxTicksSinceLastOutput, aStep.timeBefore);          //
    TEST_ASSERT_EQUAL(0U, theMotionCtrl.theSampleTime.sampleIndex);        // and the sampleTime should not proceed

    // add a motionItem to the motionBuffer but keep motionState == stopped, we should get default stepSignals, and the sampleTime should not proceed
    simplifiedMotion aMotion;
    aMotion.setForTest(0U);
    theMotionCtrl.append(aMotion);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(0U, aStep.signals);                                  // we should get default stepSignals..
    TEST_ASSERT_EQUAL(maxTicksSinceLastOutput, aStep.timeBefore);          //
    TEST_ASSERT_EQUAL(0U, theMotionCtrl.theSampleTime.sampleIndex);        // and the sampleTime should not proceed

    // Push some steps in the stepSignals, and verify they are coming out, ie. theStepSignals are always running, even when motionControl is stopped
    theMotionCtrl.theStepSignals.stepForward(0);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();        //
    TEST_ASSERT_EQUAL(0x01, aStep.signals);                     //
    TEST_ASSERT_EQUAL(1, aStep.timeBefore);                     //
    aStep = theMotionCtrl.calcNextStepperMotorSignals();        //
    TEST_ASSERT_EQUAL(0x00, aStep.signals);                     //
    TEST_ASSERT_EQUAL(1, aStep.timeBefore);                     //
}

void nextStep_emptyMotionBuffer() {
    step aStep;
    motionCtrl theMotionCtrl;
    theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(maxTicksSinceLastOutput);

    theMotionCtrl.theMotionCtrlState = motionState::running;

    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(0U, aStep.signals);                                  // we should get default stepSignals..
    TEST_ASSERT_EQUAL(maxTicksSinceLastOutput, aStep.timeBefore);          //
    TEST_ASSERT_EQUAL(0U, theMotionCtrl.theSampleTime.sampleIndex);        // and the sampleTime should be reset to zeroes
    TEST_ASSERT_EQUAL(motionState::stopped, theMotionCtrl.getState());
}

void nextStep_sequenceMotionItems() {
    step aStep;
    motionCtrl theMotionCtrl;
    theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(std::numeric_limits<uint32_t>::max());        // not interested in (interfering) these dummy stepsignals, so disabling them
    theMotionCtrl.theSampleTime.setminStepPulseWidth(1.0f / 8.0f);
    simplifiedMotion aMotion;
    aMotion.setForTest(10U);
    theMotionCtrl.append(aMotion);
    aMotion.setForTest(11U);
    theMotionCtrl.append(aMotion);
    aMotion.setForTest(10U);
    theMotionCtrl.append(aMotion);
    TEST_ASSERT_EQUAL(03, theMotionCtrl.theMotionBuffer.getLevel());
    theMotionCtrl.start();

    for (int i = 0; i < 12; i++) {
        aStep = theMotionCtrl.calcNextStepperMotorSignals();        //
    }
}

void setSimplifiedMotion() {
    step aStep;
    point currentPosition;
    simplifiedMotion aMotion;
    aMotion.set(currentPosition, axis::X, 123.4, 12.3);
    motionCtrl theMotionCtrl;
    theMotionCtrl.append(aMotion);
    theMotionCtrl.start();

    for (int i = 0; i < 16; i++) {
        aStep = theMotionCtrl.calcNextStepperMotorSignals();        //
    }
    theMotionCtrl.stop();
    for (int i = 0; i < 4; i++) {
        aStep = theMotionCtrl.calcNextStepperMotorSignals();        //
    }
    theMotionCtrl.flushMotionBuffer();
    theMotionCtrl.getMachinePosition(currentPosition);
    aMotion.set(currentPosition, axis::X, -23.4, 12.3);
    theMotionCtrl.append(aMotion);
    theMotionCtrl.start();
    TEST_MESSAGE("missing test");
}

void test_optimize() {
    TEST_MESSAGE("missing test");
    TEST_IGNORE();
}

void test_optimizePair() {
    TEST_MESSAGE("missing test");
    TEST_IGNORE();
}

void test_vJunction() {
    TEST_MESSAGE("missing test");
    TEST_IGNORE();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialize);
    RUN_TEST(setSimplifiedMotion);

    // --- Testing generation of stepSignals ---

    RUN_TEST(nextStep_notRunning);                 // 1. output defaults when empty buffer
    RUN_TEST(nextStep_emptyMotionBuffer);          //
    RUN_TEST(nextStep_sequenceMotionItems);        //

    // 2. output defaults when non-empty buffer but stopped
    // 3. output steps when running and motionItems in the buffer
    // 4. proceed to next motionItem after completing the current one
    // 5. go to Stopped after completing last motionItem
    // 6. go to Stopped after passing tStop
    // 7. time should proceed only when not stopped = running or stopping
    // 8. stepsignals should always proceed

    // --- Testing motionBuffer optimization ---

    RUN_TEST(test_optimize);
    RUN_TEST(test_optimizePair);
    RUN_TEST(test_vJunction);
    UNITY_END();
}