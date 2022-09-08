#include <unity.h>
#include "motionbuffer.h"

// ----------------------------------
// --- to be tested API           ---
// ----------------------------------

// bool isEmpty() const;             //
// bool isFull() const;              //
// uint32_t getLevel() const;        //
// uint32_t push();                  // pushes 1 item on the buffer and returns an index to it..
// void pop();                       // pops the oldest item on from the buffer and adjusts indexes and level
// motion* getHeadPtr();             //

// ----------------------------------
// --- globals                    ---
// ----------------------------------

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void initialization() {
    motionBuffer theMotionBuffer;
    TEST_ASSERT_EQUAL(0U, theMotionBuffer.head);
    TEST_ASSERT_EQUAL(0U, theMotionBuffer.level);
    TEST_ASSERT_EQUAL(0U, theMotionBuffer.maxLevel);
    TEST_ASSERT_TRUE(theMotionBuffer.isEmpty());
    TEST_ASSERT_FALSE(theMotionBuffer.isFull());
}

void pushAndPop() {
    motionBuffer theMotionBuffer;
    for (uint32_t i = 0; i < motionBuffer::length - 1; i++) {
        uint32_t newItemIndex = theMotionBuffer.push();
        TEST_ASSERT_EQUAL(i, newItemIndex);
        TEST_ASSERT_EQUAL(i + 1, theMotionBuffer.getLevel());
        TEST_ASSERT_FALSE(theMotionBuffer.isEmpty());
        TEST_ASSERT_EQUAL(0U, theMotionBuffer.head);
    }
    {
        uint32_t newItemIndex = theMotionBuffer.push();
        TEST_ASSERT_EQUAL(motionBuffer::length - 1, newItemIndex);
        TEST_ASSERT_EQUAL(motionBuffer::length, theMotionBuffer.getLevel());
        TEST_ASSERT_FALSE(theMotionBuffer.isEmpty());
        TEST_ASSERT_TRUE(theMotionBuffer.isFull());
        TEST_ASSERT_EQUAL(0U, theMotionBuffer.head);
    }

    for (uint32_t i = 0; i < motionBuffer::length - 1; i++) {
        theMotionBuffer.pop();
        TEST_ASSERT_EQUAL(motionBuffer::length - (i + 1), theMotionBuffer.getLevel());
        TEST_ASSERT_FALSE(theMotionBuffer.isEmpty());
        TEST_ASSERT_FALSE(theMotionBuffer.isFull());
        TEST_ASSERT_EQUAL(i + 1, theMotionBuffer.head);
    }
    {
        theMotionBuffer.pop();
        TEST_ASSERT_EQUAL(0U, theMotionBuffer.getLevel());
        TEST_ASSERT_TRUE(theMotionBuffer.isEmpty());
        TEST_ASSERT_FALSE(theMotionBuffer.isFull());
        TEST_ASSERT_EQUAL(0U, theMotionBuffer.head);
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(initialization);
    RUN_TEST(pushAndPop);
    UNITY_END();
}