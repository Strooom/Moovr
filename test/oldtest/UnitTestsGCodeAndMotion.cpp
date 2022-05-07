#include "CppUnitTest.h"
#include "eventbuffer.h"
#include "gcode.h"
#include "machineProperties.h"
#include "motion.h"
#include "motionCtrl.h"

#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestsGcodeAndMotion {
TEST_CLASS (C01_gCode2MotionTests) {
  public:
    TEST_METHOD (Segment) {
        gCode aParser;
        gCodeParserResult theResult;
        motion anItem;
        machineProperties theMachineProperties;
        theMachineProperties.motors.jMax                   = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t)axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t)axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t)axis::X] = 100.0F;

        overrides theOverrides;

        aParser.initialize();
        aParser.getBlock("G1 X80 F1200");        // This segment, in combination with above machineProperties, results into 1 s for each of the 7 phases of the motion
        aParser.parseBlock(theResult);
        anItem.set(theResult, theMachineProperties, motionStrategy::maximizeSpeed, theOverrides);
        Assert::IsTrue(motionType::FeedLinear == anItem.type, L"001");
        Assert::IsTrue(80.0F == anItem.trajectory.length, L"002");
        Assert::IsTrue(7.0F == anItem.speedProfile.duration, L"003");
    }

    TEST_METHOD (Segment2) {
        gCode aParser;
        gCodeParserResult theResult;
        motion anItem;
        machineProperties theMachineProperties;
        theMachineProperties.motors.jMax                   = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t)axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t)axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t)axis::X] = 100.0F;

        overrides theOverrides;

        aParser.initialize();
        aParser.getBlock("G1 X30 F600");        // This segment and feedrate result in an empty phase 2 and 6
        aParser.parseBlock(theResult);
        anItem.set(theResult, theMachineProperties, motionStrategy::maximizeSpeed, theOverrides);
        Assert::IsTrue(motionType::FeedLinear == anItem.type, L"001");
        Assert::IsTrue(30.0F == anItem.trajectory.length, L"002");
        Assert::IsTrue(5.0F == anItem.speedProfile.duration, L"003");
    }

    TEST_METHOD (Segment3) {
        gCode aParser;
        gCodeParserResult theResult;
        motion anItem;
        machineProperties theMachineProperties;
        theMachineProperties.motors.jMax                   = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t)axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t)axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t)axis::X] = 100.0F;

        overrides theOverrides;

        aParser.initialize();
        aParser.getBlock("G1 X20 F600");        // This segment and feedrate result in an empty phase 2, 4 and 6
        aParser.parseBlock(theResult);
        anItem.set(theResult, theMachineProperties, motionStrategy::maximizeSpeed, theOverrides);
        Assert::IsTrue(motionType::FeedLinear == anItem.type, L"001");
        Assert::IsTrue(20.0F == anItem.trajectory.length, L"002");
        Assert::IsTrue(4.0F == anItem.speedProfile.duration, L"003");
    }

    TEST_METHOD (Segment4) {
        gCode aParser;
        gCodeParserResult theResult;
        motion anItem;
        machineProperties theMachineProperties;
        theMachineProperties.motors.jMax                   = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t)axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t)axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t)axis::X] = 100.0F;

        overrides theOverrides;

        aParser.initialize();
        aParser.getBlock("G1 X10 F600");
        aParser.parseBlock(theResult);
        anItem.set(theResult, theMachineProperties, motionStrategy::maximizeSpeed, theOverrides);
        Assert::IsTrue(motionType::FeedLinear == anItem.type, L"001");
        Assert::IsTrue(10.0F == anItem.trajectory.length, L"002");
        Assert::IsTrue(10.0F == (anItem.speedProfile.left.length + anItem.speedProfile.mid.length + anItem.speedProfile.right.length), L"004");
    }

    TEST_METHOD (Segment5) {
        gCode aParser;
        gCodeParserResult theResult;
        motion anItem;
        machineProperties theMachineProperties;
        theMachineProperties.motors.jMax                   = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t)axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t)axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t)axis::X] = 100.0F;

        overrides theOverrides;

        aParser.initialize();
        aParser.getBlock("G1 X5 F600");
        aParser.parseBlock(theResult);
        anItem.set(theResult, theMachineProperties, motionStrategy::maximizeSpeed, theOverrides);
        Assert::IsTrue(motionType::FeedLinear == anItem.type, L"001");
        Assert::IsTrue(5.0F == anItem.trajectory.length, L"002");
        Assert::IsTrue(5.0F == (anItem.speedProfile.left.length + anItem.speedProfile.mid.length + anItem.speedProfile.right.length), L"004");
    }

    TEST_METHOD (T10_s) {
        gCode aParser;
        gCodeParserResult theResult;
        motion theMotion;
        machineProperties theMachineProperties;
        theMachineProperties.motors.jMax                   = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t)axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t)axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t)axis::X] = 100.0F;

        overrides theOverrides;

        aParser.initialize();
        aParser.getBlock("G1 X80 F1200");
        //aParser.getBlock("G1 X30 F600");
        //aParser.getBlock("G1 X20 F600");
        //aParser.getBlock("G1 X10 F600");
        //aParser.getBlock("G1 X80 F1200");
        //aParser.getBlock("G1 X80 F1200");

        
        aParser.parseBlock(theResult);
        theMotion.set(theResult, theMachineProperties, motionStrategy::maximizeSpeed, theOverrides);

        //Assert::IsTrue(theMotion.s(3.5F) == 40.0F, L"001");

        char tmp[4096];
        uint32_t outputLength;
        outputLength = theMotion.toString(tmp);
    }
};

TEST_CLASS (C02_motion2StepsTests) {
  public:
    TEST_METHOD (T01) {
        eventBuffer theEventBuffer;
        gCode aParser;
        gCodeParserResult theResult;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMachineProperties.motors.jMax                   = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t)axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t)axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t)axis::X] = 100.0F;

        aParser.initialize();
        aParser.getBlock("G1 X80 F1200");        // This segment, in combination with above machineProperties, results into 1 s for each of the 7 phases of the motion
        aParser.parseBlock(theResult);

        Assert::IsTrue(theCtrl.theMotionBuffer.isEmpty(), L"001");
        theCtrl.append(theResult);
        Assert::IsFalse(theCtrl.theMotionBuffer.isEmpty(), L"001");

       theCtrl.run();
        std::cout << std::endl;
    }
};

TEST_CLASS (C03_motionOptimizingTests) {
  public:
    TEST_METHOD (T01) {
        eventBuffer theEventBuffer;
        gCode aParser;
        gCodeParserResult theResult;
        machineProperties theMachineProperties;
        overrides theOverrides;
        stepBuffer theStepBuffer;
        motionCtrl theCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);
        theMachineProperties.motors.jMax                   = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t)axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t)axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t)axis::X] = 100.0F;

        aParser.initialize();
        aParser.getBlock("G1 X80 F1200");        // This segment, in combination with above machineProperties, results into 1 s for each of the 7 phases of the motion
        aParser.parseBlock(theResult);

        Assert::IsTrue(theCtrl.theMotionBuffer.isEmpty(), L"001");
        theCtrl.append(theResult);
        Assert::IsFalse(theCtrl.theMotionBuffer.isEmpty(), L"001");

        theCtrl.run();
        std::cout << std::endl;
    }
};

}        // namespace UnitTestsGcodeAndMotion
