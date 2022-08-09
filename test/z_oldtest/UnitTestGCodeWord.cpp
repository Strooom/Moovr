#include "CppUnitTest.h"
#include "gcodeword.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestsGcodeWord {
TEST_CLASS (C01_gCodeWord) {
  public:
    TEST_METHOD (T01_constructor) {
        gCodeWord aWord;
        Assert::AreEqual((uint32_t)aWord.letter, 0U);
        Assert::AreEqual(aWord.number, 0.0);
        Assert::AreEqual(aWord.intNumber, 0U);
    }
};
}        // namespace UnitTestsGcodeWord
