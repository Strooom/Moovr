#include "CppUnitTest.h"        // this gives you access to the C++ Unit Test framework
#include "machineproperties.h"
#include "logging.h"
#include "stepbuffer.h"

uLog theLog;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestEvent {
TEST_CLASS (C01_StepBuffer) {
  public:
    TEST_METHOD (T01_Construction) {
        stepBuffer theBuffer;
        uint32_t level = theBuffer.getBufferLevel();
        uint32_t btime = theBuffer.getBufferTimeInTicks();

        Assert::IsTrue(theBuffer.getBufferLevel() >= theBuffer.minBufferLevel);
        Assert::IsTrue(theBuffer.getBufferTimeInTicks() >= minStepBufferTotalTimeTicks);
    }

    TEST_METHOD (T02_Level) {
        stepBuffer theBuffer;
        uint32_t level = theBuffer.getBufferLevel();
        theBuffer.write(step{0, 0});
        Assert::IsTrue((level + 1) == theBuffer.getBufferLevel());
        step aStep = theBuffer.read();
        Assert::IsTrue(level == theBuffer.getBufferLevel());
    }

    TEST_METHOD (T03_Head) {
        stepBuffer theBuffer;
        uint32_t head = theBuffer.bufferHead;
        theBuffer.write(step{0, 0});
        Assert::IsTrue(head == theBuffer.bufferHead);
        step aStep = theBuffer.read();
        Assert::IsTrue((head + 1) % theBuffer.bufferLength == theBuffer.bufferHead);
    }

    TEST_METHOD (T04_Time) {
        stepBuffer theBuffer;
        uint32_t btime = theBuffer.getBufferTimeInTicks();
        theBuffer.write(step{123, 0});
        Assert::IsTrue((btime + 123) == theBuffer.getBufferTimeInTicks());
        step aStep = theBuffer.read();
        Assert::IsTrue((btime + 123 - aStep.timeBefore) == theBuffer.getBufferTimeInTicks());
    }

    TEST_METHOD (T05_IndexWrapping) {
        stepBuffer theBuffer;
        uint32_t head  = theBuffer.bufferHead;
        uint32_t level = theBuffer.getBufferLevel();
        for (uint32_t i = 0; i < theBuffer.bufferLength; i++) {
            theBuffer.write(step{1, 0});
            step aStep = theBuffer.read();
        }
        Assert::IsTrue(level == theBuffer.getBufferLevel());
        Assert::IsTrue(head == theBuffer.bufferHead);
    }
};
}        // namespace UnitTestEvent
