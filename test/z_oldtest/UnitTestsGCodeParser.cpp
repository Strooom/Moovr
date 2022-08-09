#include "CppUnitTest.h"
#include "gcode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestsGcode {

TEST_CLASS (C02_gCodeParser) {
  public:

    TEST_METHOD (T03_gCodeParseResultType) {
        gCode aParser;
        gCodeParserResult theResult;

        aParser.initialize();
        aParser.getBlock("F1200");
        aParser.parseBlock(theResult);
        Assert::IsTrue(gCodeParserResult::ParseResultType::OkContextUpdateOnly == theResult.theParseResultType);

        aParser.initialize();
        aParser.getBlock("G1 X100 F1200");
        aParser.parseBlock(theResult);
        Assert::IsTrue(gCodeParserResult::ParseResultType::OkContextUpdateAndMotion == theResult.theParseResultType);
        Assert::IsTrue(motionType::FeedLinear == theResult.motion.theMotionType);
    }

    TEST_METHOD (T04_gCodeParseResultTraverse) {
        gCode aParser;
        gCodeParserResult theResult;

        aParser.initialize();
        aParser.getBlock("G0 X1000");
        aParser.parseBlock(theResult);
        Assert::IsTrue(motionType::Traverse == theResult.motion.theMotionType, L"001");
        Assert::IsTrue(1000.0 == theResult.motion.trajectory.length, L"002");
        Assert::IsTrue(0.0 == theResult.motion.trajectory.startPosition[(uint8_t)axis::X], L"003");
        Assert::IsTrue(1000.0 == theResult.motion.trajectory.delta[(uint8_t)axis::X], L"004");
        Assert::IsTrue(1000 < theResult.motion.speedProfile.vFeed, L"005");
    }

    TEST_METHOD (T05_gCodeParseResultFeedLinear) {
        gCode aParser;
        gCodeParserResult theResult;

        aParser.initialize();
        aParser.getBlock("G1 X100 F1200");
        aParser.parseBlock(theResult);
        Assert::IsTrue(motionType::FeedLinear == theResult.motion.theMotionType);
        Assert::IsTrue(100.0 == theResult.motion.trajectory.length);
        Assert::IsTrue(0.0 == theResult.motion.trajectory.startPosition[(uint8_t)axis::X]);
        Assert::IsTrue(100.0 == theResult.motion.trajectory.delta[(uint8_t)axis::X]);
        Assert::IsTrue(20.0 == theResult.motion.speedProfile.vFeed);
    }

    TEST_METHOD (T06_gCodeParseResultError) {
        gCode aParser;
        gCodeParserResult theResult;

        aParser.initialize();
        aParser.getBlock("G0 G2 G3");
        aParser.parseBlock(theResult);
        Assert::IsTrue(gCodeParserResult::ParseResultType::Error == theResult.theParseResultType, L"001");
        Assert::IsTrue(gCodeParserResult::Error::ModalGroupCollision == theResult.error, L"002");

        aParser.initialize();
        aParser.getBlock("X1 X2");
        aParser.parseBlock(theResult);
        Assert::IsTrue(gCodeParserResult::ParseResultType::Error == theResult.theParseResultType, L"011");
        Assert::IsTrue(gCodeParserResult::Error::ValueWordCollision == theResult.error, L"012");
    }
};
}        // namespace gcodetests
