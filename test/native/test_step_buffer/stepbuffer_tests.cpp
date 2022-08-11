#include <unity.h>
#include "stepbuffer.h"

void setUp(void) {}           //  before test
void tearDown(void) {}        // after test
void noInterrupts() {}        // mock for unitTesting
void interrupts() {}          // idem

void initialization() {
    constexpr uint32_t someMinimumTotalTime{100U};
    constexpr uint32_t someMinimumLevel{4U};
    stepBuffer theBuffer = stepBuffer(someMinimumTotalTime, someMinimumLevel);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.head);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(someMinimumLevel, theBuffer.getLevel());
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(someMinimumTotalTime, theBuffer.getTimeInTicks());
    TEST_ASSERT_FALSE(theBuffer.needsFilling());
}

void read_write() {
    // writing
    constexpr uint32_t someMinimumTotalTime{100U};
    constexpr uint32_t someMinimumLevel{4U};
    stepBuffer theBuffer = stepBuffer(someMinimumTotalTime, someMinimumLevel);
    constexpr uint32_t stepDuration{500U};        //
    step aStep{stepDuration, 0};
    uint32_t totalTimeBefore = theBuffer.getTimeInTicks();
    uint32_t levelBefore     = theBuffer.getLevel();
    theBuffer.write(aStep);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32((totalTimeBefore + stepDuration), theBuffer.getTimeInTicks());
    TEST_ASSERT_EQUAL_UINT32((levelBefore + 1), theBuffer.getLevel());

    // reading
    aStep = theBuffer.read();
    TEST_ASSERT_EQUAL_UINT32(1U, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32((totalTimeBefore + stepDuration - aStep.timeBefore), theBuffer.getTimeInTicks());
    TEST_ASSERT_EQUAL_UINT32((levelBefore), theBuffer.getLevel());
}

void indexWrapping() {
    constexpr uint32_t someMinimumTotalTime{100U};
    constexpr uint32_t someMinimumLevel{4U};
    stepBuffer theBuffer = stepBuffer(someMinimumTotalTime, someMinimumLevel);
    uint32_t head        = theBuffer.head;
    uint32_t level       = theBuffer.getLevel();
    for (uint32_t i = 0; i < theBuffer.length; i++) {
        theBuffer.write(step{1, 0});
        step aStep = theBuffer.read();
    }
    TEST_ASSERT_EQUAL_UINT32(level, theBuffer.getLevel());
    TEST_ASSERT_EQUAL_UINT32(head, theBuffer.head);
}

void needsFilling() {
    constexpr uint32_t someMinimumTotalTime{100U};
    constexpr uint32_t someMinimumLevel{4U};
    stepBuffer theBuffer = stepBuffer(someMinimumTotalTime, someMinimumLevel);

    while (theBuffer.getLevel() > 0) {
        (void)theBuffer.read();
    }
    while (theBuffer.getLevel() < (someMinimumLevel + 1)) {
        theBuffer.write(step{0, 0});
        TEST_ASSERT_TRUE(theBuffer.needsFilling());
    }

    while (theBuffer.getLevel() > 0) {
        (void)theBuffer.read();
    }
    while (theBuffer.getLevel() < (someMinimumLevel - 1)) {
        theBuffer.write(step{someMinimumTotalTime, 0});
        TEST_ASSERT_TRUE(theBuffer.needsFilling());
    }

    theBuffer.write(step{someMinimumTotalTime, 0});
    TEST_ASSERT_FALSE(theBuffer.needsFilling());
}

void underflow() {
    constexpr uint32_t someMinimumTotalTime{100U};
    constexpr uint32_t someMinimumLevel{4U};
    stepBuffer theBuffer = stepBuffer(someMinimumTotalTime, someMinimumLevel);

    while (theBuffer.getLevel() > 0) {
        (void)theBuffer.read();
    }
    TEST_ASSERT_EQUAL_UINT32(event::none, theBuffer.getLastError());
    (void)theBuffer.read();
    TEST_ASSERT_EQUAL_UINT32(event::stepBufferUnderflow, theBuffer.getLastError());
    TEST_ASSERT_EQUAL_UINT32(event::none, theBuffer.getLastError());
}

void overflow() {
    constexpr uint32_t someMinimumTotalTime{100U};
    constexpr uint32_t someMinimumLevel{4U};
    stepBuffer theBuffer = stepBuffer(someMinimumTotalTime, someMinimumLevel);

    while (theBuffer.getLevel() < theBuffer.length) {
        theBuffer.write(step{10, 10});
    }
    TEST_ASSERT_EQUAL_UINT32(event::none, theBuffer.getLastError());
    theBuffer.write(step{10, 10});
    TEST_ASSERT_EQUAL_UINT32(event::stepBufferOverflow, theBuffer.getLastError());
    TEST_ASSERT_EQUAL_UINT32(event::none, theBuffer.getLastError());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    RUN_TEST(read_write);
    RUN_TEST(needsFilling);
    RUN_TEST(underflow);
    RUN_TEST(overflow);
    RUN_TEST(indexWrapping);
    UNITY_END();
}
