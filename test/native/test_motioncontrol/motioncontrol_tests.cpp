#include <unity.h>
#include "simplifiedmotion.h"
#include "motionctrl.h"
#include "machineproperties.h"
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

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void initialize() {
    motionCtrl theMotionCtrl;
    TEST_ASSERT_FALSE(theMotionCtrl.isRunning());
    TEST_ASSERT_EQUAL(motionState::stopped, theMotionCtrl.getState());
    // TODO : test default value for overrides

    
    TEST_ASSERT_EQUAL_UINT32(0U, theMotionCtrl.theMotionBuffer.head);
    TEST_ASSERT_EQUAL_UINT32(0U, theMotionCtrl.theMotionBuffer.getLevel());
    // TODO : test initialization of motionBuffer
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