#include <unity.h>
#include "eventbuffer.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test
void noInterrupts() {}        // mock for unitTesting
void interrupts() {}          // idem

void initialization() {
    eventBuffer theBuffer = eventBuffer();
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.levelMax);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.getLevelMax());
    TEST_ASSERT_FALSE(theBuffer.hasEvents());
}

void ignoreEventNone() {
    eventBuffer theBuffer = eventBuffer();
    theBuffer.pushEvent(event::none);
    TEST_ASSERT_FALSE(theBuffer.hasEvents());
}

void pushPop() {
    eventBuffer theBuffer = eventBuffer();
    theBuffer.pushEvent(event::allMotionsCompleted);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32(1U, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(1U, theBuffer.levelMax);
    TEST_ASSERT_TRUE(theBuffer.hasEvents());

    (void)theBuffer.popEvent();
    TEST_ASSERT_EQUAL_UINT32(1U, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(1U, theBuffer.levelMax);
}

void boundariesUnderflow() {
    eventBuffer theBuffer = eventBuffer();
    event anEvent;
    anEvent = theBuffer.popEvent();
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.levelMax);
    TEST_ASSERT_EQUAL(event::none, anEvent);
    TEST_ASSERT_EQUAL(event::eventBufferUnderflow, theBuffer.getLastError());
}

void boundariesOverflow() {
    eventBuffer theBuffer = eventBuffer();
    for (int i = 0; i < theBuffer.length; i++) {
        theBuffer.pushEvent(event::allMotionsCompleted);
    }
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32(theBuffer.length, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(theBuffer.length, theBuffer.levelMax);

    theBuffer.pushEvent(event::allMotionsCompleted);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32(theBuffer.length, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(theBuffer.length, theBuffer.levelMax);
    TEST_ASSERT_EQUAL(event::eventBufferOverflow, theBuffer.getLastError());
}

void test_initialize() {
    eventBuffer theBuffer = eventBuffer();
    theBuffer.pushEvent(event::allMotionsCompleted);
    (void)theBuffer.popEvent();
    theBuffer.initialize();
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.head);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.level);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.levelMax);
    TEST_ASSERT_EQUAL_UINT32(0U, theBuffer.getLevelMax());
    TEST_ASSERT_FALSE(theBuffer.hasEvents());
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    RUN_TEST(ignoreEventNone);
    RUN_TEST(pushPop);
    RUN_TEST(boundariesUnderflow);
    RUN_TEST(boundariesOverflow);
    RUN_TEST(test_initialize);
    UNITY_END();
}