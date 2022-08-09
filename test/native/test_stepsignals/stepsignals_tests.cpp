#include <unity.h>
#include "stepsignals.h"
#include "axis.h"

void setUp(void) {
}

void tearDown(void) {
}

void initialization() {
    stepSignals theSignals;
    theSignals.setMaxTicksSinceLastOutput(10U);

    TEST_ASSERT_EQUAL_UINT32(0, theSignals.dirSetup);
    TEST_ASSERT_EQUAL_UINT32(0, theSignals.stepRise);
    TEST_ASSERT_EQUAL_UINT32(0, theSignals.stepFall);
    TEST_ASSERT_EQUAL_UINT32(0, theSignals.lastOutput);
    TEST_ASSERT_EQUAL_UINT32(0, theSignals.ticksSinceLastOutput);
    TEST_ASSERT_FALSE(theSignals.isModified());
    TEST_ASSERT_FALSE(theSignals.isTimedOut());
}

void stepForward() {
    stepSignals theSignals;
    theSignals.stepForward(0);
    TEST_ASSERT_EQUAL_UINT32(0b00, theSignals.dirSetup);
    TEST_ASSERT_EQUAL_UINT32(0b01, theSignals.stepRise);
    TEST_ASSERT_EQUAL_UINT32(0b00, theSignals.stepFall);
}

void stepForwardMultipleAxis() {
    TEST_ASSERT_MESSAGE(nmbrAxis >= 3, "this test assumes 3 or more axis");
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
    TEST_ASSERT_MESSAGE(nmbrAxis >= 3, "this test assumes 3 or more axis");
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

void isModified() {
    stepSignals theSignals;
    TEST_ASSERT_FALSE(theSignals.isModified());
    theSignals.stepRise = 0x2;
    theSignals.stepFall = 0x3;
    TEST_ASSERT_FALSE(theSignals.isModified());
    theSignals.dirSetup = 0x1;
    TEST_ASSERT_TRUE(theSignals.isModified());
    theSignals.output();
    TEST_ASSERT_FALSE(theSignals.isModified());
}

void isTimedOut() {
    constexpr uint32_t maxValueTest{4U};
    stepSignals theSignals;
    theSignals.setMaxTicksSinceLastOutput(maxValueTest);
    for (int i = 0; i < (maxValueTest - 1); i++) {
        theSignals.next();
    }
    TEST_ASSERT_FALSE(theSignals.isTimedOut());
    theSignals.next();
    TEST_ASSERT_TRUE(theSignals.isTimedOut());
}

void boundsCheck() {
    {
        stepSignals theSignals;
        theSignals.stepForward(nmbrAxis + 1);
        TEST_ASSERT_EQUAL_UINT32(0, theSignals.dirSetup);
        TEST_ASSERT_EQUAL_UINT32(0, theSignals.stepRise);
        TEST_ASSERT_EQUAL_UINT32(0, theSignals.stepFall);
    }
    {
        stepSignals theSignals;
        theSignals.stepBackward(nmbrAxis + 1);
        TEST_ASSERT_EQUAL_UINT32(0, theSignals.dirSetup);
        TEST_ASSERT_EQUAL_UINT32(0, theSignals.stepRise);
        TEST_ASSERT_EQUAL_UINT32(0, theSignals.stepFall);
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    RUN_TEST(stepForward);
    RUN_TEST(stepForwardMultipleAxis);
    RUN_TEST(stepBackward);
    RUN_TEST(stepBackwardMultipleAxis);
    RUN_TEST(shifting);
    RUN_TEST(isModified);
    RUN_TEST(isTimedOut);
    RUN_TEST(boundsCheck);
    UNITY_END();
}
