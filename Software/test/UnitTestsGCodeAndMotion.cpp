#include "CppUnitTest.h"
#include "gcode.h"
#include "motionitem.h"
#include "machineproperties.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace gcodemotiontests
{
TEST_CLASS(gCode2MotionTests)
    {
public:
    TEST_METHOD(Segment)
        {
        gCode aParser;
        gCodeParserResult theResult;
        MotionItem anItem;
        MachineProperties theMachineProperties;
        theMachineProperties.motors.jMax = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t) axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t) axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t) axis::X] = 100.0F;

        Overrides theOverrides;

        aParser.initialize();
        aParser.getBlock("G1 X80 F1200");	// This segment, in combination with above machineproperties, results into 1 s for each of the 7 phases of the motion
        aParser.parseBlock(theResult);
        anItem.set(theResult, theMachineProperties, MotionStrategy::maximizeSpeed, theOverrides);
        Assert::IsTrue(MotionType::FeedLinear == anItem.theType, L"001");
        Assert::IsTrue(80.0F == anItem.theTrajectory.length, L"002");
        Assert::IsTrue(7.0F == anItem.theSpeedProfile.duration, L"003");
        }

    TEST_METHOD(Segment2)
        {
        gCode aParser;
        gCodeParserResult theResult;
        MotionItem anItem;
        MachineProperties theMachineProperties;
        theMachineProperties.motors.jMax = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t) axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t) axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t) axis::X] = 100.0F;

        Overrides theOverrides;

        aParser.initialize();
        aParser.getBlock("G1 X30 F600");	// This segment and feedrate result in an empty phase 2 and 6
        aParser.parseBlock(theResult);
        anItem.set(theResult, theMachineProperties, MotionStrategy::maximizeSpeed, theOverrides);
        Assert::IsTrue(MotionType::FeedLinear == anItem.theType, L"001");
        Assert::IsTrue(30.0F == anItem.theTrajectory.length, L"002");
        Assert::IsTrue(5.0F == anItem.theSpeedProfile.duration, L"003");
        }

    TEST_METHOD(Segment3)
        {
        gCode aParser;
        gCodeParserResult theResult;
        MotionItem anItem;
        MachineProperties theMachineProperties;
        theMachineProperties.motors.jMax = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t) axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t) axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t) axis::X] = 100.0F;

        Overrides theOverrides;

        aParser.initialize();
        aParser.getBlock("G1 X20 F600"); // This segment and feedrate result in an empty phase 2, 4 and 6
        aParser.parseBlock(theResult);
        anItem.set(theResult, theMachineProperties, MotionStrategy::maximizeSpeed, theOverrides);
        Assert::IsTrue(MotionType::FeedLinear == anItem.theType, L"001");
        Assert::IsTrue(20.0F == anItem.theTrajectory.length, L"002");
        Assert::IsTrue(4.0F == anItem.theSpeedProfile.duration, L"003");
        }

    TEST_METHOD(Segment4)
        {
        gCode aParser;
        gCodeParserResult theResult;
        MotionItem anItem;
        MachineProperties theMachineProperties;
        theMachineProperties.motors.jMax = 10.0F;
        theMachineProperties.motors.aMax[(uint8_t) axis::X] = 10.0F;
        theMachineProperties.motors.dMax[(uint8_t) axis::X] = -10.0F;
        theMachineProperties.motors.vMax[(uint8_t) axis::X] = 100.0F;

        Overrides theOverrides;

        aParser.initialize();
        aParser.getBlock("G1 X10 F600");
        aParser.parseBlock(theResult);
        anItem.set(theResult, theMachineProperties, MotionStrategy::maximizeSpeed, theOverrides);
        Assert::IsTrue(MotionType::FeedLinear == anItem.theType, L"001");
        Assert::IsTrue(10.0F == anItem.theTrajectory.length, L"002");
        Assert::IsTrue(10.0F == (anItem.theSpeedProfile.left.length + anItem.theSpeedProfile.mid.length + anItem.theSpeedProfile.right.length), L"004");
        }

	TEST_METHOD(Segment5)
	{
		gCode aParser;
		gCodeParserResult theResult;
		MotionItem anItem;
		MachineProperties theMachineProperties;
		theMachineProperties.motors.jMax = 10.0F;
		theMachineProperties.motors.aMax[(uint8_t) axis::X] = 10.0F;
		theMachineProperties.motors.dMax[(uint8_t) axis::X] = -10.0F;
		theMachineProperties.motors.vMax[(uint8_t) axis::X] = 100.0F;

		Overrides theOverrides;

		aParser.initialize();
		aParser.getBlock("G1 X5 F600");
		aParser.parseBlock(theResult);
		anItem.set(theResult, theMachineProperties, MotionStrategy::maximizeSpeed, theOverrides);
		Assert::IsTrue(MotionType::FeedLinear == anItem.theType, L"001");
		Assert::IsTrue(5.0F == anItem.theTrajectory.length, L"002");
		Assert::IsTrue(5.0F == (anItem.theSpeedProfile.left.length + anItem.theSpeedProfile.mid.length + anItem.theSpeedProfile.right.length), L"004");
	}
    };

}
