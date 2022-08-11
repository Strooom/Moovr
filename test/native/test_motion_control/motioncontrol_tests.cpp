
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

// step calculateStepperSignals();

// ----------------------------------
// --- globals                    ---
// ----------------------------------

machineProperties theMachineProperties;        //
eventBuffer theEventBuffer;                    //

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void initialize() {
    motionCtrl theMotionCtrl;
    TEST_ASSERT_FALSE(theMotionCtrl.isRunning());
    TEST_ASSERT_EQUAL(motionState::stopped, theMotionCtrl.getState());
    TEST_ASSERT_EQUAL(1.0f, theMotionCtrl.theOverrides.feedOverride);
    TEST_ASSERT_EQUAL(1.0f, theMotionCtrl.theOverrides.spindleOverride);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialize);
    //    RUN_TEST(append);
    //    RUN_TEST();
    //    RUN_TEST();
    //    RUN_TEST();
    //    RUN_TEST();
    //    RUN_TEST();
    //    RUN_TEST();
    //    RUN_TEST();
    //    RUN_TEST();
    UNITY_END();
}