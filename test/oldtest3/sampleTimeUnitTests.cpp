#include "CppUnitTest.h"        // this gives you access to the C++ Unit Test framework
#include "logging.h"
#include "sampletime.h"

extern uLog theLog;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace sampleTimeUnitTests {

TEST_CLASS (C01_sampleTime) {
  public:
    TEST_METHOD (T01_Construction) {
        motionBuffer theBuffer = motionBuffer();
        sampleTime theTime(theBuffer);
        Assert::AreEqual(0U, theTime.sampleIndex);
        Assert::AreEqual(0.0F, theTime.sampleZeroOffset);
        Assert::AreEqual(0.0F, theTime.timeInMotionFloat);
    }

    TEST_METHOD (T02_Next_Reset) {
        motionBuffer theBuffer = motionBuffer();
        sampleTime theTime(theBuffer);
        theTime.next();
        Assert::AreEqual(1U, theTime.sampleIndex);
        theTime.next();
        Assert::AreEqual(2U, theTime.sampleIndex);
        theTime.next();
        Assert::AreEqual(3U, theTime.sampleIndex);
        theTime.reset();
        Assert::AreEqual(0U, theTime.sampleIndex);
    }
};
}        // namespace sampleTimeUnitTests