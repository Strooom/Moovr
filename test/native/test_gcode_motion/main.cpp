
#include <unity.h>
#include <math.h>
#include <limits>

#include "eventbuffer.h"              // motionControl relies on an external dependency : there must be a global instance of eventBuffer where it can notify/push events that occured
#include "machineproperties.h"        // motionControl relies on an external dependency : there must be a global instance of machineProperties where it can read settings/properties
#include "motionctrl.h"
#include "gcode.h"

// ----------------------------------
// --- globals                    ---
// ----------------------------------

machineProperties theMachineProperties;        //
eventBuffer theEventBuffer;                    //

void setUp(void) {
}

void tearDown(void) {}        // after test

// test_001 : Add a single motion, start() and check if the correct steps are coming out, and after all steps are taken, the ctrl stops and sends the related events.

void test_001() {
    theEventBuffer.initialize();
    theMachineProperties.setForTest(1U);
    motionCtrl theMotionCtrl;
    theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(1000U);        // not interested in (interfering) these dummy stepsignals, so disabling them, //  theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(std::numeric_limits<uint32_t>::max());
    theMotionCtrl.theSampleTime.setminStepPulseWidth(1.0f / 32.0f);

    simplifiedMotion aMotion;
    gCode theInterpreter;

    theInterpreter.getBlockFromString("G0 X2");        // 2 mm move at vmax = 1 mm/s, amax = 1mm/s2 -> 3 seconds
    theInterpreter.interpreteBlock(aMotion);
    theMotionCtrl.append(aMotion);

    theMotionCtrl.start();

    step aStep;

    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(18, aStep.timeBefore);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(11, aStep.timeBefore);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(7, aStep.timeBefore);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(7, aStep.timeBefore);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(7, aStep.timeBefore);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(7, aStep.timeBefore);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(7, aStep.timeBefore);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(11, aStep.timeBefore);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();

    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(4U, theEventBuffer.level);
    TEST_ASSERT_EQUAL(event::motionStarted, theEventBuffer.theEventBuffer[0]);
    TEST_ASSERT_EQUAL(event::motionStopped, theEventBuffer.theEventBuffer[1]);
    TEST_ASSERT_EQUAL(event::motionCompleted, theEventBuffer.theEventBuffer[2]);
    TEST_ASSERT_EQUAL(event::allMotionsCompleted, theEventBuffer.theEventBuffer[3]);
}

// test_002 : add two motions, start() and check if after the correct number of steps, the control proceeds to the second motion

void test_002() {
    theEventBuffer.initialize();
    theMachineProperties.setForTest(1U);
    motionCtrl theMotionCtrl;
    theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(1000U);        // not interested in (interfering) these dummy stepsignals, so disabling them, //  theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(std::numeric_limits<uint32_t>::max());
    theMotionCtrl.theSampleTime.setminStepPulseWidth(1.0f / 32.0f);

    simplifiedMotion aMotion;
    gCode theInterpreter;
    step aStep;

    theInterpreter.getBlockFromString("G0 X2");        // 2 mm move at vmax = 1 mm/s, amax = 1mm/s2 -> 3 seconds
    theInterpreter.interpreteBlock(aMotion);
    theMotionCtrl.append(aMotion);

    theInterpreter.getBlockFromString("G0 X0");
    theInterpreter.interpreteBlock(aMotion);
    theMotionCtrl.append(aMotion);

    theMotionCtrl.start();

    for (int i = 0; i < 16; i++) {
        aStep = theMotionCtrl.calcNextStepperMotorSignals();
    }

    TEST_ASSERT_EQUAL(1U, theEventBuffer.level);
    TEST_ASSERT_EQUAL(event::motionStarted, theEventBuffer.theEventBuffer[0]);

    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(2U, theEventBuffer.level);
    TEST_ASSERT_EQUAL(event::motionCompleted, theEventBuffer.theEventBuffer[1]);

    // TODO : add some asserts to check that everything is properly initialised after switching to the second motion
}

// test_003 : add a single motion, start(), take a number of steps, then stop() and see the motion come to a stop and send the related events
// then check that steps are dummies
// then resume and check that steps are back to normal and total number of steps is correct for the length of the motion segment

void test_003() {
    theEventBuffer.initialize();
    theMachineProperties.setForTest(1U);
    motionCtrl theMotionCtrl;
    theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(1000U);        // not interested in (interfering) these dummy stepsignals, so disabling them, //  theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(std::numeric_limits<uint32_t>::max());
    theMotionCtrl.theSampleTime.setminStepPulseWidth(1.0f / 32.0f);

    simplifiedMotion aMotion;
    gCode theInterpreter;
    step aStep;

    theInterpreter.getBlockFromString("G0 X10");        // 10 mm move at vmax = 1 mm/s, amax = 1mm/s2 -> 11 seconds
    theInterpreter.interpreteBlock(aMotion);
    theMotionCtrl.append(aMotion);

    theMotionCtrl.start();

    for (int i = 0; i < 8; i++) {
        aStep = theMotionCtrl.calcNextStepperMotorSignals();
    }

    theMotionCtrl.stop();

    for (int i = 0; i < 6; i++) {
        aStep = theMotionCtrl.calcNextStepperMotorSignals();
    }

    TEST_ASSERT_FALSE(theMotionCtrl.isRunning());
    TEST_ASSERT_EQUAL(3U, theEventBuffer.level);
    TEST_ASSERT_EQUAL(event::motionStarted, theEventBuffer.theEventBuffer[0]);
    TEST_ASSERT_EQUAL(event::stopRequested, theEventBuffer.theEventBuffer[1]);
    TEST_ASSERT_EQUAL(event::motionStopped, theEventBuffer.theEventBuffer[2]);

    theMotionCtrl.start();

    for (int i = 0; i < 8; i++) {
        aStep = theMotionCtrl.calcNextStepperMotorSignals();
    }

    TEST_ASSERT_TRUE(theMotionCtrl.isRunning());
}

// test_004 : add a single motion, start(), take a number of steps, then adjust feed override to see the motion being recalculated

void optimize() {
    theEventBuffer.initialize();
    theMachineProperties.setForTest(1U);
    motionCtrl theMotionCtrl;
    theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(1000U);        // not interested in (interfering) these dummy stepsignals, so disabling them, //  theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(std::numeric_limits<uint32_t>::max());
    theMotionCtrl.theSampleTime.setminStepPulseWidth(1.0f / 32.0f);

    simplifiedMotion aMotion;
    gCode theInterpreter;
    step aStep;

    theInterpreter.getBlockFromString("G1 X10 F30");
    theInterpreter.interpreteBlock(aMotion);
    theMotionCtrl.append(aMotion);

    theMotionCtrl.start();
    TEST_ASSERT_EQUAL(20.5f, theMotionCtrl.theMotionBuffer.getHead().speedProfile.duration);

    theMotionCtrl.theOverrides.feedOverride = 0.5f;
    theMotionCtrl.optimize();
    TEST_ASSERT_EQUAL(40.25f, theMotionCtrl.theMotionBuffer.getHead().speedProfile.duration);

    theMotionCtrl.theOverrides.feedOverride = 2.0f;
    theMotionCtrl.optimize();
    TEST_ASSERT_EQUAL(11.0f, theMotionCtrl.theMotionBuffer.getHead().speedProfile.duration);

    theMotionCtrl.theOverrides.feedOverride = 3.0f;        // this 300# exceeds vMax, so it will limit to
    theMotionCtrl.optimize();
    TEST_ASSERT_EQUAL(11.0f, theMotionCtrl.theMotionBuffer.getHead().speedProfile.duration);
}

void G4() {
    theEventBuffer.initialize();
    theMachineProperties.setForTest(1U);
    motionCtrl theMotionCtrl;
    theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(1000U);        // not interested in (interfering) these dummy stepsignals, so disabling them, //  theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(std::numeric_limits<uint32_t>::max());
    theMotionCtrl.theSampleTime.setminStepPulseWidth(1.0f / 32.0f);

    simplifiedMotion aMotion;
    gCode theInterpreter;
    step aStep;

    theInterpreter.getBlockFromString("G4 P0.5");
    theInterpreter.interpreteBlock(aMotion);
    theMotionCtrl.append(aMotion);
    theMotionCtrl.start();

    for (int i = 0; i < 2; i++) {
        aStep = theMotionCtrl.calcNextStepperMotorSignals();
    }

    TEST_ASSERT_FALSE(theMotionCtrl.isRunning());
    TEST_ASSERT_EQUAL(4U, theEventBuffer.level);
    TEST_ASSERT_EQUAL(event::motionStarted, theEventBuffer.theEventBuffer[0]);
    TEST_ASSERT_EQUAL(event::motionStopped, theEventBuffer.theEventBuffer[1]);
    TEST_ASSERT_EQUAL(event::motionCompleted, theEventBuffer.theEventBuffer[2]);
    TEST_ASSERT_EQUAL(event::allMotionsCompleted, theEventBuffer.theEventBuffer[3]);

    // TODO : can we feedhold in the middle of a G4, and then resume ?
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_001);
    RUN_TEST(test_002);
    RUN_TEST(test_003);
    RUN_TEST(optimize);
    RUN_TEST(G4);
    UNITY_END();
}