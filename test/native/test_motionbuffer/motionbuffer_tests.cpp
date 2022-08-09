#include <unity.h>

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void initialization() {
    //     {
    //         SpeedProfileAccDec motionPartLeft  = SpeedProfileAccDec(0.0f, 1.0f, 1.0f, -1.0f, 10.0f);        // constructing a motionpart at beginning
    //         SpeedProfileAccDec motionPartRight = SpeedProfileAccDec(0.0f, 1.0f, 1.0f, -1.0f, 10.0f);        // constructing a motionpart at ending
    //         motionPartLeft.calculate(MotionSpeedProfileOrder::firstOrder);                                  // calculate all parameters
    //         motionPartRight.calculate(MotionSpeedProfileOrder::firstOrder);                                 // calculate all parameters

    //         MotionItem theMotionItem;        // construct a motionITEM

    //         theMotionItem.fromParts(motionPartLeft, motionPartRight, 0.0f, 0.0f);        // populate the data from the two PARTS

    //         theTest.printValue(sizeof(theMotionItem), "memory footprint of MotionItem [bytes]");

    //         theTest.assert(1, theMotionItem.vMax == 0.0f, "someTest");
    //     }
    //     {
    //         TEST_CLASS (C01_MotionBuffer) {
    //   public:
    //     TEST_METHOD (T01_constructor) {
    //         motionBuffer theBuffer;
    //         Assert::AreEqual(0U, theBuffer.level);
    //         Assert::AreEqual(0U, theBuffer.head);
    //         Assert::AreEqual(0U, theBuffer.levelMax);
    //         Assert::IsTrue(theBuffer.isEmpty());
    //     }
    //     TEST_METHOD (T02_initialization) {
    //     }

    //     TEST_METHOD (T03_PushAndPop) {
    //         motionBuffer theMotionBuffer;
    //         uint32_t newItemIndex = theMotionBuffer.push();
    //         Assert::IsTrue(newItemIndex == 0, L"002");
    //         Assert::IsFalse(theMotionBuffer.isEmpty(), L"003");
    //         Assert::IsTrue(theMotionBuffer.level == 1, L"004");

    //         theMotionBuffer.pop();
    //         Assert::IsTrue(theMotionBuffer.isEmpty(), L"005");
    //     }

    //     TEST_METHOD (T04_DetectFull) {
    //         motionBuffer theMotionBuffer;
    //         for (uint32_t i = 0; i < theMotionBuffer.length; i++) {
    //             (void)theMotionBuffer.push();
    //         }
    //         Assert::IsTrue(theMotionBuffer.isFull());
    //         Assert::IsFalse(theMotionBuffer.isEmpty());

    //         for (uint32_t i = 0; i < theMotionBuffer.length; i++) {
    //             theMotionBuffer.pop();
    //         }
    //         Assert::IsFalse(theMotionBuffer.isFull());
    //         Assert::IsTrue(theMotionBuffer.isEmpty());
    //     }
    // };

    //     }
    TEST_FAIL();
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    // RUN_TEST(initialization);
    UNITY_END();
}