#include <unity.h>
#include "sampletime.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

constexpr uint32_t outputTimerFrequency = 60'000U;
constexpr float minStepPulseWidth       = 1.0F / outputTimerFrequency;

void initialization() {
    sampleTime theTime;
    theTime.setminStepPulseWidth(minStepPulseWidth);
    TEST_ASSERT_EQUAL_UINT32(0U, theTime.sampleIndex);
    TEST_ASSERT_EQUAL_FLOAT(0.0F, theTime.sampleZeroOffset);
    TEST_ASSERT_EQUAL_FLOAT(minStepPulseWidth, theTime.minStepPulseWidth);
}

void next() {
    sampleTime theTime;
    theTime.next();
    TEST_ASSERT_EQUAL_UINT32(1U, theTime.sampleIndex);
    theTime.next();
    TEST_ASSERT_EQUAL_UINT32(2U, theTime.sampleIndex);
    theTime.next();
    TEST_ASSERT_EQUAL_UINT32(3U, theTime.sampleIndex);
    theTime.initialize();
    TEST_ASSERT_EQUAL_UINT32(0U, theTime.sampleIndex);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    RUN_TEST(next);
    UNITY_END();
}