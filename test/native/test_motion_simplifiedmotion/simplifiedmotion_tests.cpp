#include <unity.h>
#include "simplifiedmotion.h"
#include "point.h"
// ----------------------------------
// --- to be tested API           ---
// ----------------------------------

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void testInitialize() {
    simplifiedMotion aMotion;
    TEST_ASSERT_EQUAL(motionType::none, aMotion.type);
    TEST_ASSERT_EQUAL(0.0, aMotion.trajectory.length);
    for (uint32_t axisIndex = 0; axisIndex < nmbrAxis; axisIndex++) {
        TEST_ASSERT_EQUAL(0.0, aMotion.trajectory.startPosition[axisIndex]);
        TEST_ASSERT_EQUAL(0.0, aMotion.trajectory.delta[axisIndex]);
    }
}

void testSetForHoming() {
    point currentPosition;
    simplifiedMotion aMotion;
    aMotion.set(currentPosition, axis::X, 123.4, 12.3);
    TEST_ASSERT_EQUAL(123.4, aMotion.trajectory.length);
    TEST_ASSERT_EQUAL(0.0, aMotion.trajectory.startPosition[0]);
    TEST_ASSERT_EQUAL(123.4, aMotion.trajectory.delta[0]);
    TEST_ASSERT_EQUAL(12.3, aMotion.speedProfile.vFeed);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(testInitialize);
    RUN_TEST(testSetForHoming);
    UNITY_END();
}