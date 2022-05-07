#include "CppUnitTest.h"        // this gives you access to the C++ Unit Test framework
#include "logging.h"
#include "machineproperties.h"
#include "stepbuffer.h"
#include "stepsignals.h"

extern uLog theLog;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace stepBufferUnitTests {
TEST_CLASS (C01_StepBuffer) {
  public:
    TEST_METHOD (T01_ConstructionInitialization) {
        stepBuffer theBuffer;
        Assert::IsTrue(minBufferLevel <= theBuffer.level);
        Assert::AreEqual(0U, theBuffer.head);
        Assert::IsTrue(minStepBufferTotalTimeTicks <= theBuffer.timeInTicks);
    }

    TEST_METHOD (T02_Level) {
        stepBuffer theBuffer;
        uint32_t level = theBuffer.getLevel();
        theBuffer.write(step{0, 0});
        Assert::IsTrue((level + 1) == theBuffer.getLevel());
        step aStep = theBuffer.read();
        Assert::IsTrue(level == theBuffer.getLevel());
    }

    TEST_METHOD (T03_Head) {
        stepBuffer theBuffer;
        uint32_t head = theBuffer.head;
        theBuffer.write(step{0, 0});
        Assert::IsTrue(head == theBuffer.head);
        step aStep = theBuffer.read();
        Assert::IsTrue((head + 1) % theBuffer.length == theBuffer.head);
    }

    TEST_METHOD (T04_Time) {
        stepBuffer theBuffer;
        uint32_t btime = theBuffer.getTimeInTicks();
        theBuffer.write(step{123, 0});
        Assert::IsTrue((btime + 123) == theBuffer.getTimeInTicks());
        step aStep = theBuffer.read();
        Assert::IsTrue((btime + 123 - aStep.timeBefore) == theBuffer.getTimeInTicks());
    }

    TEST_METHOD (T05_IndexWrapping) {
        stepBuffer theBuffer;
        uint32_t head  = theBuffer.head;
        uint32_t level = theBuffer.getLevel();
        for (uint32_t i = 0; i < theBuffer.length; i++) {
            theBuffer.write(step{1, 0});
            step aStep = theBuffer.read();
        }
        Assert::IsTrue(level == theBuffer.getLevel());
        Assert::IsTrue(head == theBuffer.head);
    }

    TEST_METHOD (T06_NeedsFilling) {
        stepBuffer theBuffer;
        Assert::IsFalse(theBuffer.needsFilling());
        (void) theBuffer.read();
        Assert::IsTrue(theBuffer.needsFilling());
        while (theBuffer.level < theBuffer.length) {
            theBuffer.write(step{0, 0});
        }
        Assert::IsFalse(minStepBufferTotalTimeTicks <= theBuffer.timeInTicks);
        Assert::IsFalse(theBuffer.needsFilling());
    }

};
}        // namespace UnitTestStepBuffer
