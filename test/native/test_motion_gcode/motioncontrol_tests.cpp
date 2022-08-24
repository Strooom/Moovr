
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
    gCode theParser;

    theParser.theBlock.getBlockFromString("G0 X2");        // 2 mm move at vmax = 1 mm/s, amax = 1mm/s2 -> 3 seconds
    theParser.parseBlock(aMotion);
    theMotionCtrl.append(aMotion);

    theMotionCtrl.start();

    step aStep;

    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(19, aStep.timeBefore);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(10, aStep.timeBefore);
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
    TEST_ASSERT_EQUAL(8, aStep.timeBefore);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(11, aStep.timeBefore);
    aStep = theMotionCtrl.calcNextStepperMotorSignals();

    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(3U, theEventBuffer.level);
    TEST_ASSERT_EQUAL(event::motionStopped, theEventBuffer.theEventBuffer[0]);
    TEST_ASSERT_EQUAL(event::motionCompleted, theEventBuffer.theEventBuffer[1]);
    TEST_ASSERT_EQUAL(event::allMotionsCompleted, theEventBuffer.theEventBuffer[2]);
}

// test_002 : add two motions, start() and check if after the correct number of steps, the control proceeds to the second motion

void test_002() {
    theEventBuffer.initialize();
    theMachineProperties.setForTest(1U);
    motionCtrl theMotionCtrl;
    theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(1000U);        // not interested in (interfering) these dummy stepsignals, so disabling them, //  theMotionCtrl.theStepSignals.setMaxTicksSinceLastOutput(std::numeric_limits<uint32_t>::max());
    theMotionCtrl.theSampleTime.setminStepPulseWidth(1.0f / 32.0f);

    simplifiedMotion aMotion;
    gCode theParser;
    step aStep;

    theParser.theBlock.getBlockFromString("G0 X2");        // 2 mm move at vmax = 1 mm/s, amax = 1mm/s2 -> 3 seconds
    theParser.parseBlock(aMotion);
    theMotionCtrl.append(aMotion);

    theParser.theBlock.getBlockFromString("G0 X0");
    theParser.parseBlock(aMotion);
    theMotionCtrl.append(aMotion);

    theMotionCtrl.start();

    for (int i = 0; i < 16; i++) {
        aStep = theMotionCtrl.calcNextStepperMotorSignals();
    }

    TEST_ASSERT_FALSE(theEventBuffer.hasEvents());
    aStep = theMotionCtrl.calcNextStepperMotorSignals();
    TEST_ASSERT_EQUAL(1U, theEventBuffer.level);
    TEST_ASSERT_EQUAL(event::motionCompleted, theEventBuffer.theEventBuffer[0]);

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
    gCode theParser;
    step aStep;

    theParser.theBlock.getBlockFromString("G0 X10");        // 10 mm move at vmax = 1 mm/s, amax = 1mm/s2 -> 11 seconds
    theParser.parseBlock(aMotion);
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
    TEST_ASSERT_EQUAL(1U, theEventBuffer.level);
    TEST_ASSERT_EQUAL(event::motionStopped, theEventBuffer.theEventBuffer[0]);

    theMotionCtrl.start();

    for (int i = 0; i < 8; i++) {
        aStep = theMotionCtrl.calcNextStepperMotorSignals();
    }

    TEST_ASSERT_TRUE(theMotionCtrl.isRunning());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_001);
    RUN_TEST(test_002);
    RUN_TEST(test_003);
    UNITY_END();
}