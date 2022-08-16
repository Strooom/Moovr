#include <unity.h>
#include <limits>
#include "gcode.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void reset() {
    gCode theParser;
    simplifiedMotion theMotion;
    theParser.reset();

    TEST_ASSERT_EQUAL(gCodeParseResultType::EmptyBlock, theParser.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theParser.theError);
    TEST_ASSERT_EQUAL(0.0, theParser.theState.currentPosition[0]);
    TEST_ASSERT_EQUAL(0, theParser.theBlock.getNmbrWords());

    theParser.theBlock.getBlockFromString("G0 X10");        // 10 mm X-axis move
    theParser.parseBlock(theMotion);

    TEST_ASSERT_EQUAL(gCodeParseResultType::OkContextUpdateAndMotion, theParser.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theParser.theError);
    TEST_ASSERT_EQUAL(10.0, theParser.theState.currentPosition[0]);

    theParser.reset();
    TEST_ASSERT_EQUAL(gCodeParseResultType::EmptyBlock, theParser.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theParser.theError);
    TEST_ASSERT_EQUAL(0.0, theParser.theState.currentPosition[0]);
    TEST_ASSERT_EQUAL(0, theParser.theBlock.getNmbrWords());
}

void parseBlock() {
    gCode theParser;
    simplifiedMotion theMotion;

    theParser.reset();
    theParser.theBlock.getBlockFromString("G0 X10");        // 10 mm X-axis move
    theParser.parseBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::OkContextUpdateAndMotion, theParser.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theParser.theError);
    TEST_ASSERT_EQUAL(motionType::traverse, theMotion.type);
    // TEST_ASSERT_EQUAL(std::numeric_limits<double>::infinity(), theMotion.speedProfile.vFeed);
    TEST_ASSERT_EQUAL(10.0, theMotion.trajectory.delta[(uint8_t)axis::X]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Y]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Z]);
    TEST_ASSERT_EQUAL(10.0, theMotion.trajectory.length);

    theParser.reset();
    theParser.theBlock.getBlockFromString("G1 X10 F600");        // 10 mm X-axis move @ feedspeed
    theParser.parseBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::OkContextUpdateAndMotion, theParser.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theParser.theError);
    TEST_ASSERT_EQUAL(motionType::feedLinear, theMotion.type);
    TEST_ASSERT_EQUAL(10.0, theMotion.speedProfile.vFeed);
    TEST_ASSERT_EQUAL(10.0, theMotion.trajectory.delta[(uint8_t)axis::X]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Y]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Z]);
    TEST_ASSERT_EQUAL(10.0, theMotion.trajectory.length);

    theParser.reset();
    theParser.theBlock.getBlockFromString("G2 X10 Y10 I10 J0 F1200");        // 10 mm arc CW, from 0,0, to 10,10, center = 10,0
    theParser.parseBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::OkContextUpdateAndMotion, theParser.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theParser.theError);
    TEST_ASSERT_EQUAL(motionType::feedHelicalCW, theMotion.type);
    TEST_ASSERT_EQUAL(20.0, theMotion.speedProfile.vFeed);
    TEST_ASSERT_EQUAL(10.0, theMotion.trajectory.delta[(uint8_t)axis::X]);
    TEST_ASSERT_EQUAL(10.0, theMotion.trajectory.delta[(uint8_t)axis::Y]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Z]);
    TEST_ASSERT_EQUAL((5 * gCode::pi), theMotion.trajectory.length);

    theParser.reset();
    theParser.theBlock.getBlockFromString("G3 X10 Y10 I0 J10 F2400");        // 10 mm arc CCW, from 0,0, to 10,10, center = 0,10
    theParser.parseBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::OkContextUpdateAndMotion, theParser.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theParser.theError);
    TEST_ASSERT_EQUAL(motionType::feedHelicalCCW, theMotion.type);
    TEST_ASSERT_EQUAL(40.0, theMotion.speedProfile.vFeed);
    TEST_ASSERT_EQUAL(10.0, theMotion.trajectory.delta[(uint8_t)axis::X]);
    TEST_ASSERT_EQUAL(10.0, theMotion.trajectory.delta[(uint8_t)axis::Y]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Z]);
    TEST_ASSERT_EQUAL((5 * gCode::pi), theMotion.trajectory.length);

// TODO : add the case with a R radius instead of IJK offset

}

void detectModalGroupCollisions() {
    gCode theParser;
    simplifiedMotion theMotion;

    theParser.reset();
    theParser.theBlock.getBlockFromString("G0 G1");
    theParser.parseBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::Error, theParser.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::ModalGroupCollision, theParser.theError);

    theParser.reset();
    theParser.theBlock.getBlockFromString("G2 G3");
    theParser.parseBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::Error, theParser.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::ModalGroupCollision, theParser.theError);

    // TODO : complete with all other modal group collisions in gcode standard
}

void detectLetterValueCollisions() {
    gCode theParser;
    simplifiedMotion theMotion;

    theParser.reset();
    theParser.theBlock.getBlockFromString("G0 X0 X1 X2");
    theParser.parseBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::Error, theParser.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::ValueWordCollision, theParser.theError);
    // TODO : complete with other letter valuie collisions in gcode standard
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(reset);
    RUN_TEST(parseBlock);
    RUN_TEST(detectModalGroupCollisions);
    RUN_TEST(detectLetterValueCollisions);
    UNITY_END();
}

// theParser.initialize();
// theParser.getBlock("G21 N100 F1000");
// theParser.parseBlock(result);
// theTest.assert(51, result.error == gCodeParserResult::Error::InvalidLineNumber, "invalid line number");

// theParser.getBlock("G1 X80 F1200");
// theParser.parseBlock(theResult);
// theTrajectory.set(theResult);
// Assert::AreEqual(theTrajectory.length, 80.0F);

// aParser.getBlock("F1200");
// aParser.parseBlock(theResult);
// Assert::IsTrue(gCodeParserResult::ParseResultType::OkContextUpdateOnly == theResult.theParseResultType);

// aParser.initialize();
// aParser.getBlock("G1 X100 F1200");
// aParser.parseBlock(theResult);
// Assert::IsTrue(gCodeParserResult::ParseResultType::OkContextUpdateAndMotion == theResult.theParseResultType);
// Assert::IsTrue(motionType::FeedLinear == theResult.motion.theMotionType);

// aParser.getBlock("G0 X1000");
// aParser.parseBlock(theResult);
// Assert::IsTrue(motionType::Traverse == theResult.motion.theMotionType, L"001");
// Assert::IsTrue(1000.0 == theResult.motion.trajectory.length, L"002");
// Assert::IsTrue(0.0 == theResult.motion.trajectory.startPosition[(uint8_t)axis::X], L"003");
// Assert::IsTrue(1000.0 == theResult.motion.trajectory.delta[(uint8_t)axis::X], L"004");
// Assert::IsTrue(1000 < theResult.motion.speedProfile.vFeed, L"005");

// aParser.getBlock("G1 X100 F1200");
// aParser.parseBlock(theResult);
// Assert::IsTrue(20.0 == theResult.motion.speedProfile.vFeed);

// aParser.getBlock("G0 G2 G3");
// aParser.parseBlock(theResult);
// Assert::IsTrue(gCodeParserResult::ParseResultType::Error == theResult.theParseResultType, L"001");
// Assert::IsTrue(gCodeParserResult::Error::ModalGroupCollision == theResult.error, L"002");

// aParser.initialize();
// aParser.getBlock("X1 X2");
// aParser.parseBlock(theResult);
// Assert::IsTrue(gCodeParserResult::ParseResultType::Error == theResult.theParseResultType, L"011");
// Assert::IsTrue(gCodeParserResult::Error::ValueWordCollision == theResult.error, L"012");
