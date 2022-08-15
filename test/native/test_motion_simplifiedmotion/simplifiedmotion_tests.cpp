#include <unity.h>
#include "simplifiedmotion.h"

// ----------------------------------
// --- to be tested API           ---
// ----------------------------------

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void testTestSet() {
    {
        simplifiedMotion aMotion;
        aMotion.setForTest(20U);
        TEST_MESSAGE("Set 20");
    }
    {
        simplifiedMotion aMotion;
        aMotion.setForTest(1U);
        TEST_MESSAGE("Set 1");
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(testTestSet);
    UNITY_END();
}