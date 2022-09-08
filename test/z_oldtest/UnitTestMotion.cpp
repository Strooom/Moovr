#include "CppUnitTest.h"
#include "gcode.h"
#include "motion.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTestsMotion {
TEST_CLASS (C01_Motion) {
  public:
    TEST_METHOD (T01_constructor) {
        motion aMotion;
        Assert::IsTrue(motionType::None == aMotion.type);
    }
    TEST_METHOD (T02_initialization) {
    }

    TEST_METHOD (T01_Construction) {
        motion theMotion;
    }
    TEST_METHOD (T02_vOut) {
        motion theMotion;
        //        float vOut(float vIn, float aMax, float dMax, float length, motionStrategy strategy, motionCalculateDirection direction) const;
        Assert::AreEqual(30.0F, theMotion.vOut(10.0F, 10.0F, -10.0F, 40.0F, motionStrategy::maximizeSpeed, motionCalculateDirection::forward), 0.0001F);
        Assert::AreEqual(30.0F, theMotion.vOut(10.0F, 10.0F, -10.0F, 40.0F, motionStrategy::maximizeSpeed, motionCalculateDirection::reverse), 0.0001F);
        Assert::AreEqual(10.0F, theMotion.vOut(30.0F, 10.0F, -10.0F, 40.0F, motionStrategy::minimizeSpeed, motionCalculateDirection::forward), 0.0001F);
        Assert::AreEqual(10.0F, theMotion.vOut(30.0F, 10.0F, -10.0F, 40.0F, motionStrategy::minimizeSpeed, motionCalculateDirection::reverse), 0.0001F);
        Assert::AreEqual(00.0F, theMotion.vOut(10.0F, 10.0F, -10.0F, 40.0F, motionStrategy::minimizeSpeed, motionCalculateDirection::forward), 0.0001F);
        Assert::AreEqual(00.0F, theMotion.vOut(10.0F, 10.0F, -10.0F, 40.0F, motionStrategy::minimizeSpeed, motionCalculateDirection::reverse), 0.0001F);
    }
    TEST_METHOD (T03_vTri) {
        motion theMotion;
        //        float vTri(float vStart, float vEnd, float aMax, float dMax, float length, motionStrategy strategy) const;
        Assert::AreEqual(20.0F, theMotion.vTri(10.0F, 00.0F, 10.0F, -10.0F, 35.0F, motionStrategy::maximizeSpeed), 0.0001F);
        Assert::AreEqual(20.0F, theMotion.vTri(00.0F, 10.0F, 10.0F, -10.0F, 35.0F, motionStrategy::maximizeSpeed), 0.0001F);
        Assert::AreEqual(10.0F, theMotion.vTri(20.0F, 30.0F, 10.0F, -10.0F, 55.0F, motionStrategy::minimizeSpeed), 0.0001F);
        Assert::AreEqual(10.0F, theMotion.vTri(30.0F, 20.0F, 10.0F, -10.0F, 55.0F, motionStrategy::minimizeSpeed), 0.0001F);
        Assert::AreEqual(00.0F, theMotion.vTri(10.0F, 10.0F, 10.0F, -10.0F, 10.0F, motionStrategy::minimizeSpeed), 0.0001F);
        Assert::AreEqual(00.0F, theMotion.vTri(10.0F, 10.0F, 10.0F, -10.0F, 10.0F, motionStrategy::minimizeSpeed), 0.0001F);
    }

    TEST_METHOD (T04_Optimize) {
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
        aParser.interpreteBlock(theResult);
        anItem.set(theResult, theMachineProperties, motionStrategy::maximizeSpeed, theOverrides);

        // TODO
    }

    TEST_METHOD (T05_OptimizeCurrent) {
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
        aParser.interpreteBlock(theResult);
        anItem.set(theResult, theMachineProperties, motionStrategy::maximizeSpeed, theOverrides);
        anItem.optimizeCurrent(motionStrategy::minimizeSpeed, theOverrides, 3.5F);
    }

    TEST_METHOD (T06_OptimizeCurrent2) {
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
        aParser.getBlock("G1 X100 F1200");
        aParser.interpreteBlock(theResult);
        anItem.set(theResult, theMachineProperties, motionStrategy::maximizeSpeed, theOverrides);
        theOverrides.feedOverride = 1.5F;
        anItem.optimizeCurrent(motionStrategy::maximizeSpeed, theOverrides, 3.5F);
    }
};
}        // namespace UnitTestsMotion