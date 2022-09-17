#include <unity.h>
#include "homing.h"
#include "machineproperties.h"
#include "debouncedinput.h"
#include "motionctrl.h"
#include "eventbuffer.h"

machineProperties theMachineProperties;
debouncedInput myInputs[nmbrInputs] = {
    debouncedInput(event::limitSwitchZMaxClosed, event::limitSwitchZMaxOpened),
    debouncedInput(event::limitSwitchXMaxClosed, event::limitSwitchXMaxOpened),
    debouncedInput(event::limitSwitchYMaxClosed, event::limitSwitchYMaxOpened),
    debouncedInput(event::limitSwitchYMinClosed, event::limitSwitchYMinOpened),
    debouncedInput(event::limitSwitchZMinClosed, event::limitSwitchZMinOpened),
    debouncedInput(event::limitSwitchXMinClosed, event::limitSwitchXMinOpened),
    debouncedInput(event::emergencyStopButtonPressed, event::emergencyStopButtonReleased),
    debouncedInput(event::feedHoldResumeButtonPressed, event::feedHoldResumeButtonReleased),
    debouncedInput(event::probeSwitchClosed, event::probeSwitchOpened)};
eventBuffer theEventBuffer;
motionCtrl theMotionController;

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void initialization() {
    homingController theController;
    TEST_ASSERT_EQUAL(homingState::lost, theController.theHomingState);
    TEST_ASSERT_EQUAL(0, theController.homingSequenceIndex);
}

void testSelectAxis() {        // not selecting any axis when they are not defined in homingSequence
    homingController theController;
    for (int axisIndex = 0; axisIndex < nmbrAxis; axisIndex++) {
        theMachineProperties.homingSequence[axisIndex] = axis::nmbrAxis;
    }
    TEST_ASSERT_FALSE(theController.selectAxis());
}

void testSelectAxis2() {
    homingController theController;
    for (int axisIndex = 0; axisIndex < nmbrAxis; axisIndex++) {
        theMachineProperties.homingSequence[axisIndex] = axis::nmbrAxis;
    }
    theMachineProperties.homingSequence[0] = axis::Z;
    TEST_ASSERT_TRUE(theController.selectAxis());
    TEST_ASSERT_EQUAL(0, theController.homingSequenceIndex);
    TEST_ASSERT_EQUAL(axis::Z, theController.currentHomingAxis);
    TEST_ASSERT_EQUAL(2, theController.currentHomingAxisIndex);
}

void testSelectAxis3() {        // cannot home if the related limit switches are not present
    homingController theController;
    for (int axisIndex = 0; axisIndex < nmbrAxis; axisIndex++) {
        theMachineProperties.limits.hasLimitsMax[axisIndex] = false;
    }
    TEST_ASSERT_FALSE(theController.selectAxis());
}

// test missing limit switches
int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    RUN_TEST(testSelectAxis);
    RUN_TEST(testSelectAxis2);
    RUN_TEST(testSelectAxis3);
    UNITY_END();
}