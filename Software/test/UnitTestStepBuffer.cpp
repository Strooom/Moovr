#include "CppUnitTest.h"        // this gives you access to the C++ Unit Test framework
#include "logging.h"
#include "machineproperties.h"
#include "stepbuffer.h"
#include "stepsignals.h"

extern uLog theLog;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestStepSignals {
TEST_CLASS (C01_StepSignals) {
  public:
    TEST_METHOD (T01_Construction) {
        stepSignals theSignals;
        Assert::IsTrue(theSignals.dirSetup == 0);
        Assert::IsTrue(theSignals.stepRise == 0);
        Assert::IsTrue(theSignals.stepFall == 0);
    }

    TEST_METHOD (T02_StepForward) {
        stepSignals theSignals;
        theSignals.stepForward(0);
        Assert::IsTrue(theSignals.dirSetup == 0b00);
        Assert::IsTrue(theSignals.stepRise == 0b01);
        Assert::IsTrue(theSignals.stepFall == 0b00);
    }

    TEST_METHOD (T03_StepForwardMultipleAxis) {
        stepSignals theSignals;
        theSignals.stepForward(0);
        theSignals.stepForward(1);
        theSignals.stepForward(2);
        Assert::IsTrue(theSignals.dirSetup == 0b00000000);
        Assert::IsTrue(theSignals.stepRise == 0b00010101);
        Assert::IsTrue(theSignals.stepFall == 0b00000000);
    }

    TEST_METHOD (T04_StepBackward) {
        stepSignals theSignals;
        theSignals.stepBackward(0);
        Assert::IsTrue(theSignals.dirSetup == 0b10);
        Assert::IsTrue(theSignals.stepRise == 0b11);
        Assert::IsTrue(theSignals.stepFall == 0b10);
    }

    TEST_METHOD (T05_StepBackwardMultipleAxis) {
        stepSignals theSignals;
        theSignals.stepBackward(0);
        theSignals.stepBackward(1);
        theSignals.stepBackward(2);
        Assert::IsTrue(theSignals.dirSetup == 0b00101010);
        Assert::IsTrue(theSignals.stepRise == 0b00111111);
        Assert::IsTrue(theSignals.stepFall == 0b00101010);
    }

    TEST_METHOD (T06_Shifting) {
        stepSignals theSignals;
        theSignals.dirSetup = 0x1;
        theSignals.stepRise = 0x2;
        theSignals.stepFall = 0x3;
        theSignals.shift();
        Assert::IsTrue(theSignals.dirSetup == 2);
        Assert::IsTrue(theSignals.stepRise == 3);
        Assert::IsTrue(theSignals.stepFall == 3);
        theSignals.shift();
        Assert::IsTrue(theSignals.dirSetup == 3);
        Assert::IsTrue(theSignals.stepRise == 3);
        Assert::IsTrue(theSignals.stepFall == 3);
    }
};
}        // namespace UnitTestStepSignals

namespace UnitTestStepBuffer {
TEST_CLASS (C01_StepBuffer) {
  public:
    TEST_METHOD (T01_Construction) {
        stepBuffer theBuffer;
        uint32_t level = theBuffer.getBufferLevel();
        uint32_t btime = theBuffer.getBufferTimeInTicks();

        //Assert::IsTrue(theBuffer.getBufferLevel() >= theBuffer.minBufferLevel);
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
}        // namespace UnitTestStepBuffer
