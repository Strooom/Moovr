#include <unity.h>
#include "stepsignals.h"

void initialization() {
    stepSignals theSignals;
    TEST_ASSERT_EQUAL_UINT32(0, theSignals.dirSetup);
    TEST_ASSERT_EQUAL_UINT32(0, theSignals.stepRise);
    TEST_ASSERT_EQUAL_UINT32(0, theSignals.stepFall);
    TEST_ASSERT_EQUAL_UINT32(0, theSignals.lastOutput);
    TEST_ASSERT_EQUAL_UINT32(0, theSignals.ticksSinceLastOutput);
}

void stepForward() {
    stepSignals theSignals;
    theSignals.stepForward(0);
    TEST_ASSERT_EQUAL_UINT32(0b00, theSignals.dirSetup);
    TEST_ASSERT_EQUAL_UINT32(0b01, theSignals.stepRise);
    TEST_ASSERT_EQUAL_UINT32(0b00, theSignals.stepFall);
}

void stepForwardMultipleAxis() {
    stepSignals theSignals;
    theSignals.stepForward(0);
    theSignals.stepForward(1);
    theSignals.stepForward(2);
    TEST_ASSERT_EQUAL_UINT32(0b00000000, theSignals.dirSetup);
    TEST_ASSERT_EQUAL_UINT32(0b00010101, theSignals.stepRise);
    TEST_ASSERT_EQUAL_UINT32(0b00000000, theSignals.stepFall);
}

void stepBackward() {
    stepSignals theSignals;
    theSignals.stepBackward(0);
    TEST_ASSERT_EQUAL_UINT32(0b10, theSignals.dirSetup);
    TEST_ASSERT_EQUAL_UINT32(0b11, theSignals.stepRise);
    TEST_ASSERT_EQUAL_UINT32(0b10, theSignals.stepFall);
}

void stepBackwardMultipleAxis() {
    stepSignals theSignals;
    theSignals.stepBackward(0);
    theSignals.stepBackward(1);
    theSignals.stepBackward(2);
    TEST_ASSERT_EQUAL_UINT32(0b00101010, theSignals.dirSetup);
    TEST_ASSERT_EQUAL_UINT32(0b00111111, theSignals.stepRise);
    TEST_ASSERT_EQUAL_UINT32(0b00101010, theSignals.stepFall);
}

void shifting() {
    stepSignals theSignals;
    theSignals.dirSetup = 0x1;
    theSignals.stepRise = 0x2;
    theSignals.stepFall = 0x3;
    theSignals.next();
    TEST_ASSERT_EQUAL_UINT32(2, theSignals.dirSetup);
    TEST_ASSERT_EQUAL_UINT32(3, theSignals.stepRise);
    TEST_ASSERT_EQUAL_UINT32(3, theSignals.stepFall);
    theSignals.next();
    TEST_ASSERT_EQUAL_UINT32(3, theSignals.dirSetup);
    TEST_ASSERT_EQUAL_UINT32(3, theSignals.stepRise);
    TEST_ASSERT_EQUAL_UINT32(3, theSignals.stepFall);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    RUN_TEST(stepForward);
    RUN_TEST(stepForwardMultipleAxis);
    RUN_TEST(stepBackward);
    RUN_TEST(stepBackwardMultipleAxis);
    RUN_TEST(shifting);
    UNITY_END();
}
