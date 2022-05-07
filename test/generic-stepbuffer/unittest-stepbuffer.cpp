#include <unity.h>
#include "stepbuffer.h"

void noInterrupts() {}        // mock for unitTesting
void interrupts() {}          // idem

void initialization() {
    stepBuffer theBuffer = stepBuffer();
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.head);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(minBufferLevel, theBuffer.getLevel());
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(minStepBufferTotalTimeTicks, theBuffer.getTimeInTicks());
}

void read_write() {
    stepBuffer theBuffer = stepBuffer();
    constexpr uint32_t stepDuration{500U};        //
    step aStep{stepDuration, 0};
    uint32_t totalTimeBefore = theBuffer.getTimeInTicks();
    uint32_t levelBefore     = theBuffer.getLevel();
    theBuffer.write(aStep);
    TEST_ASSERT_EQUAL_UINT32((totalTimeBefore + stepDuration), theBuffer.getTimeInTicks());
    TEST_ASSERT_EQUAL_UINT32((levelBefore + 1), theBuffer.getLevel());
    // todo add test for reading
    // todo check head movement - does not move on write, moves on read
}

void indexWrapping() {
    stepBuffer theBuffer;
    uint32_t head  = theBuffer.head;
    uint32_t level = theBuffer.getLevel();
    for (uint32_t i = 0; i < theBuffer.length; i++) {
        theBuffer.write(step{1, 0});
        step aStep = theBuffer.read();
    }
    TEST_ASSERT_EQUAL_UINT32(level, theBuffer.getLevel());
    TEST_ASSERT_EQUAL_UINT32(head, theBuffer.head);
}

void needs_filling() {
}

void is_full() {
}

void underflow() {
}

void overflow() {
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    RUN_TEST(read_write);
    RUN_TEST(needs_filling);
    RUN_TEST(is_full);
    RUN_TEST(underflow);
    RUN_TEST(overflow);
    RUN_TEST(indexWrapping);
    UNITY_END();
}
