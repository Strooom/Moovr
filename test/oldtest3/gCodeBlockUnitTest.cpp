#include "CppUnitTest.h"
#include "gcode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace gCcodeBlockUnitTests {
TEST_CLASS (C01_gCodeBlock) {
  public:
    TEST_METHOD (T01_constructor) {
        gCodeBlock aBlock;
        Assert::IsTrue(0 == aBlock.nmbrWords);
    }
    TEST_METHOD (T02_initialization) {
        gCodeBlock aBlock;
        aBlock.nmbrWords = 1;
        aBlock.initialize();
        Assert::IsTrue(0 == aBlock.nmbrWords);
    }

    TEST_METHOD (T03_wordCount) {
        gCode aParser;
        Assert::IsTrue(0 == aParser.theBlock.nmbrWords);
        aParser.getBlock("X100 G1");
        Assert::IsTrue(2 == aParser.theBlock.nmbrWords);
        aParser.initialize();        // re-initialize
        aParser.getBlock("L9 X100 Y200 Z300 G1 I4 J5 K6 F700");
        Assert::IsTrue(9 == aParser.theBlock.nmbrWords);
    }

    TEST_METHOD (T04_wordSearch) {
        gCode aParser;
        aParser.initialize();        // re-initialize
        Assert::IsTrue(-1 == aParser.searchWord('G', 10));
        aParser.getBlock("L9 X100 Y200 Z300 G1 I4 J5 K6 F700");
        Assert::IsTrue(4 == aParser.searchWord('G', 10));
        Assert::IsTrue(-1 == aParser.searchWord('M', 10));
        aParser.removeWord(aParser.searchWord('L'));
        aParser.removeWord(aParser.searchWord('Y'));
        aParser.removeWord(aParser.searchWord('F'));
        aParser.removeWord(aParser.searchWord('M'));
        Assert::IsTrue(6 == aParser.theBlock.nmbrWords);
    }
};
}        // namespace UnitTestsGcodeBlock
