#include <unity.h>
#include "homing.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void initialization() {
    homingController theController;
    TEST_ASSERT_EQUAL(homingState::lost, theController.theHomingState);
    TEST_ASSERT_EQUAL(0, theController.homingAxisIndex);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    UNITY_END();
}