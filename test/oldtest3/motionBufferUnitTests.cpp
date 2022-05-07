#include "CppUnitTest.h"
#include "motionbuffer.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace motionBufferUnitTest {
TEST_CLASS (C01_MotionBuffer) {
  public:
    TEST_METHOD (T01_constructor) {
        motionBuffer theBuffer;
        Assert::AreEqual(0U, theBuffer.level);
        Assert::AreEqual(0U, theBuffer.head);
        Assert::AreEqual(0U, theBuffer.levelMax);
        Assert::IsTrue(theBuffer.isEmpty());
    }
    TEST_METHOD (T02_initialization) {
    }

    TEST_METHOD (T03_PushAndPop) {
        motionBuffer theMotionBuffer;
        uint32_t newItemIndex = theMotionBuffer.push();
        Assert::IsTrue(newItemIndex == 0, L"002");
        Assert::IsFalse(theMotionBuffer.isEmpty(), L"003");
        Assert::IsTrue(theMotionBuffer.level == 1, L"004");

        theMotionBuffer.pop();
        Assert::IsTrue(theMotionBuffer.isEmpty(), L"005");
    }

    TEST_METHOD (T04_DetectFull) {
        motionBuffer theMotionBuffer;
        for (uint32_t i = 0; i < theMotionBuffer.length; i++) {
            (void)theMotionBuffer.push();
        }
        Assert::IsTrue(theMotionBuffer.isFull());
        Assert::IsFalse(theMotionBuffer.isEmpty());

        for (uint32_t i = 0; i < theMotionBuffer.length; i++) {
            theMotionBuffer.pop();
        }
        Assert::IsFalse(theMotionBuffer.isFull());
        Assert::IsTrue(theMotionBuffer.isEmpty());
    }
};


}        // namespace MotionBuffer