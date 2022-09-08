#include <unity.h>
#include <limits>
#include "gcode.h"

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void reset() {
    gCode theInterpreter;
    simplifiedMotion theMotion;
    theInterpreter.reset();

    TEST_ASSERT_EQUAL(gCodeParseResultType::EmptyBlock, theInterpreter.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theInterpreter.theError);
    TEST_ASSERT_EQUAL(0.0, theInterpreter.theState.currentPosition[0]);
    TEST_ASSERT_EQUAL(0, theInterpreter.theBlock.getNmbrWords());

    theInterpreter.theBlock.getBlockFromString("G0 X10");        // 10 mm X-axis move
    theInterpreter.interpreteBlock(theMotion);

    TEST_ASSERT_EQUAL(gCodeParseResultType::OkContextUpdateAndMotion, theInterpreter.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theInterpreter.theError);
    TEST_ASSERT_EQUAL(10.0, theInterpreter.theState.currentPosition[0]);

    theInterpreter.reset();
    TEST_ASSERT_EQUAL(gCodeParseResultType::EmptyBlock, theInterpreter.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theInterpreter.theError);
    TEST_ASSERT_EQUAL(0.0, theInterpreter.theState.currentPosition[0]);
    TEST_ASSERT_EQUAL(0, theInterpreter.theBlock.getNmbrWords());
}

void interpreteBlock() {
    gCode theInterpreter;
    simplifiedMotion theMotion;

    theInterpreter.reset();
    theInterpreter.theBlock.getBlockFromString("G0 X1");        // 1 mm X-axis move
    theInterpreter.interpreteBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::OkContextUpdateAndMotion, theInterpreter.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theInterpreter.theError);
    TEST_ASSERT_EQUAL(motionType::traverse, theMotion.type);
    // TEST_ASSERT_EQUAL(std::numeric_limits<double>::infinity(), theMotion.speedProfile.vFeed);
    TEST_ASSERT_EQUAL(1.0, theMotion.trajectory.delta[(uint8_t)axis::X]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Y]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Z]);
    TEST_ASSERT_EQUAL(1.0, theMotion.trajectory.length);

    theInterpreter.reset();
    theInterpreter.theBlock.getBlockFromString("G1 X1 F60");        // 1 mm X-axis move @ feedspeed
    theInterpreter.interpreteBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::OkContextUpdateAndMotion, theInterpreter.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theInterpreter.theError);
    TEST_ASSERT_EQUAL(motionType::feedLinear, theMotion.type);
    TEST_ASSERT_EQUAL(1.0, theMotion.speedProfile.vFeed);
    TEST_ASSERT_EQUAL(1.0, theMotion.trajectory.delta[(uint8_t)axis::X]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Y]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Z]);
    TEST_ASSERT_EQUAL(1.0, theMotion.trajectory.length);

    theInterpreter.reset();
    theInterpreter.theBlock.getBlockFromString("G2 X1 Y1 I1 J0 F120");        // 1 mm arc CW, from 0,0, to 1,1, center = 1,0
    theInterpreter.interpreteBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::OkContextUpdateAndMotion, theInterpreter.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theInterpreter.theError);
    TEST_ASSERT_EQUAL(motionType::feedHelicalCW, theMotion.type);
    TEST_ASSERT_EQUAL(2.0, theMotion.speedProfile.vFeed);
    TEST_ASSERT_EQUAL(1.0, theMotion.trajectory.delta[(uint8_t)axis::X]);
    TEST_ASSERT_EQUAL(1.0, theMotion.trajectory.delta[(uint8_t)axis::Y]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Z]);
    TEST_ASSERT_EQUAL((0.5 * gCode::pi), theMotion.trajectory.length);

    theInterpreter.reset();
    theInterpreter.theBlock.getBlockFromString("G3 X1 Y1 I0 J1 F240");        // 1 mm arc CCW, from 0,0, to 1,1, center = 0,1
    theInterpreter.interpreteBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::OkContextUpdateAndMotion, theInterpreter.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::None, theInterpreter.theError);
    TEST_ASSERT_EQUAL(motionType::feedHelicalCCW, theMotion.type);
    TEST_ASSERT_EQUAL(4.0, theMotion.speedProfile.vFeed);
    TEST_ASSERT_EQUAL(1.0, theMotion.trajectory.delta[(uint8_t)axis::X]);
    TEST_ASSERT_EQUAL(1.0, theMotion.trajectory.delta[(uint8_t)axis::Y]);
    TEST_ASSERT_EQUAL(0.0, theMotion.trajectory.delta[(uint8_t)axis::Z]);
    TEST_ASSERT_EQUAL((0.5 * gCode::pi), theMotion.trajectory.length);

    // TODO : add the case with a R radius instead of IJK offset
}

void detectModalGroupCollisions() {
    gCode theInterpreter;
    simplifiedMotion theMotion;

    theInterpreter.reset();
    theInterpreter.theBlock.getBlockFromString("G0 G1");
    theInterpreter.interpreteBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::Error, theInterpreter.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::ModalGroupCollision, theInterpreter.theError);

    theInterpreter.reset();
    theInterpreter.theBlock.getBlockFromString("G2 G3");
    theInterpreter.interpreteBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::Error, theInterpreter.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::ModalGroupCollision, theInterpreter.theError);

    // TODO : complete with all other modal group collisions in gcode standard
}

void detectLetterValueCollisions() {
    gCode theInterpreter;
    simplifiedMotion theMotion;

    theInterpreter.reset();
    theInterpreter.theBlock.getBlockFromString("G0 X0 X1 X2");
    theInterpreter.interpreteBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::Error, theInterpreter.theResult);
    TEST_ASSERT_EQUAL(gCodeParseError::ValueWordCollision, theInterpreter.theError);
    // TODO : complete with other letter valuie collisions in gcode standard
}

void G1() {
    TEST_IGNORE();
}

void G2G3() {
    // test invalid ijk resulting in unknown radius...
    // test invalid R ??
    TEST_IGNORE();
}

void G4() {
    gCode theInterpreter;
    simplifiedMotion theMotion;

    theInterpreter.reset();
    theInterpreter.theBlock.getBlockFromString("G4 P0.5");
    theInterpreter.interpreteBlock(theMotion);
    TEST_ASSERT_EQUAL(gCodeParseResultType::OkContextUpdateAndMotion, theInterpreter.theResult);
    TEST_ASSERT_EQUAL(motionType::pauseAndResume, theMotion.type);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, theMotion.trajectory.length);
    TEST_ASSERT_EQUAL_DOUBLE(0.5, theMotion.speedProfile.duration);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, theMotion.speedProfile.vFeed);

    for (uint32_t axisIndex = 0; axisIndex < nmbrAxis; ++axisIndex) {
        TEST_ASSERT_EQUAL_DOUBLE(0.0, theMotion.trajectory.startPosition[axisIndex]);
        TEST_ASSERT_EQUAL_DOUBLE(0.0, theMotion.trajectory.delta[axisIndex]);
    }
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(reset);
    RUN_TEST(interpreteBlock);
    RUN_TEST(detectModalGroupCollisions);
    RUN_TEST(detectLetterValueCollisions);
    RUN_TEST(G1);
    RUN_TEST(G2G3);
    RUN_TEST(G4);
    UNITY_END();
}

// theInterpreter.initialize();
// theInterpreter.getBlock("G21 N100 F1000");
// theInterpreter.interpreteBlock(result);
// theTest.assert(51, result.error == gCodeParserResult::Error::InvalidLineNumber, "invalid line number");

// theInterpreter.getBlock("G1 X80 F1200");
// theInterpreter.interpreteBlock(theResult);
// theTrajectory.set(theResult);
// Assert::AreEqual(theTrajectory.length, 80.0F);

// aParser.getBlock("F1200");
// aParser.interpreteBlock(theResult);
// Assert::IsTrue(gCodeParserResult::ParseResultType::OkContextUpdateOnly == theResult.theParseResultType);

// aParser.initialize();
// aParser.getBlock("G1 X100 F1200");
// aParser.interpreteBlock(theResult);
// Assert::IsTrue(gCodeParserResult::ParseResultType::OkContextUpdateAndMotion == theResult.theParseResultType);
// Assert::IsTrue(motionType::FeedLinear == theResult.motion.theMotionType);

// aParser.getBlock("G0 X1000");
// aParser.interpreteBlock(theResult);
// Assert::IsTrue(motionType::Traverse == theResult.motion.theMotionType, L"001");
// Assert::IsTrue(1000.0 == theResult.motion.trajectory.length, L"002");
// Assert::IsTrue(0.0 == theResult.motion.trajectory.startPosition[(uint8_t)axis::X], L"003");
// Assert::IsTrue(1000.0 == theResult.motion.trajectory.delta[(uint8_t)axis::X], L"004");
// Assert::IsTrue(1000 < theResult.motion.speedProfile.vFeed, L"005");

// aParser.getBlock("G1 X100 F1200");
// aParser.interpreteBlock(theResult);
// Assert::IsTrue(20.0 == theResult.motion.speedProfile.vFeed);

// aParser.getBlock("G0 G2 G3");
// aParser.interpreteBlock(theResult);
// Assert::IsTrue(gCodeParserResult::ParseResultType::Error == theResult.theParseResultType, L"001");
// Assert::IsTrue(gCodeParserResult::Error::ModalGroupCollision == theResult.error, L"002");

// aParser.initialize();
// aParser.getBlock("X1 X2");
// aParser.interpreteBlock(theResult);
// Assert::IsTrue(gCodeParserResult::ParseResultType::Error == theResult.theParseResultType, L"011");
// Assert::IsTrue(gCodeParserResult::Error::ValueWordCollision == theResult.error, L"012");
