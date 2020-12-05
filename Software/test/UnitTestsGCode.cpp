#include "CppUnitTest.h"
#include "gcode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace gcodetests
{
TEST_CLASS(gCodeBlockTests)
    {
public:
    TEST_METHOD(constructorInitialization)
        {
        gCodeBlock aBlock;
        Assert::IsTrue(0 == aBlock.nmbrWords);
        }
    TEST_METHOD(initialization)
        {
        gCodeBlock aBlock;
        aBlock.nmbrWords = 1;
        aBlock.initialize();
        Assert::IsTrue(0 == aBlock.nmbrWords);
        }
    };

TEST_CLASS(gCodeParserTests)
    {
public:
    TEST_METHOD(wordCount)
        {
        gCode aParser;
        Assert::IsTrue(0 == aParser.theBlock.nmbrWords);
        aParser.getBlock("X100 G1");
        Assert::IsTrue(2 == aParser.theBlock.nmbrWords);
        aParser.initialize();	// re-initialize
        aParser.getBlock("L9 X100 Y200 Z300 G1 I4 J5 K6 F700");
        Assert::IsTrue(9 == aParser.theBlock.nmbrWords);
        }
    TEST_METHOD(wordSearch)
        {
        gCode aParser;
        aParser.initialize();	// re-initialize
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
    TEST_METHOD(gCodeParseResultType)
        {
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
        Assert::IsTrue(MotionType::FeedLinear == theResult.motion.theMotionType);
        }

    TEST_METHOD(gCodeParseResultTraverse)
        {
        gCode aParser;
        gCodeParserResult theResult;

        aParser.initialize();
        aParser.getBlock("G0 X1000");
        aParser.parseBlock(theResult);
        Assert::IsTrue(MotionType::Traverse == theResult.motion.theMotionType, L"001");
        Assert::IsTrue(1000.0 == theResult.motion.trajectory.length, L"002");
        Assert::IsTrue(0.0 == theResult.motion.trajectory.startPosition[(uint8_t) axis::X], L"003");
        Assert::IsTrue(1000.0 == theResult.motion.trajectory.delta[(uint8_t) axis::X], L"004");
        Assert::IsTrue(1000 < theResult.motion.speedProfile.vFeed, L"005");
        }

    TEST_METHOD(gCodeParseResultFeedLinear)
        {
        gCode aParser;
        gCodeParserResult theResult;

        aParser.initialize();
        aParser.getBlock("G1 X100 F1200");
        aParser.parseBlock(theResult);
        Assert::IsTrue(MotionType::FeedLinear == theResult.motion.theMotionType);
        Assert::IsTrue(100.0 == theResult.motion.trajectory.length);
        Assert::IsTrue(0.0 == theResult.motion.trajectory.startPosition[(uint8_t)axis::X]);
        Assert::IsTrue(100.0 == theResult.motion.trajectory.delta[(uint8_t) axis::X]);
        Assert::IsTrue(20.0 == theResult.motion.speedProfile.vFeed);
        }

    TEST_METHOD(gCodeParseResultError)
        {
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
}

