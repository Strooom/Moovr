// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <Arduino.h>
#include <UnitTest.h>
#include "general.h"
#include "speedprofile.h"
#include "motionitem.h"
#include "gcode.h"
#include "machineproperties.h"

UnitTest theTest;

void setup()
    {
    theTest.init();
    theTest.start("MotionPart Unit Tests");
        {
        SpeedProfileAccDec theSpeedProfile = SpeedProfileAccDec(1.0f, 3.0f, 1.0f, -1.0f, 1.0f);	// constructing a motionpart with certain boundaries
        theTest.printValue(static_cast<int32_t>(sizeof(theSpeedProfile)), "memory footprint of SpeedProfileAccDec [bytes]");
        // Are all member values properly initialized by the constructor ?
        theTest.assert(10, (theSpeedProfile.vStart == 1.0f) && (theSpeedProfile.vEnd == 3.0f) && (theSpeedProfile.aMax == 1.0f) && (theSpeedProfile.dMax == -1.0f) && (theSpeedProfile.jMax == 1.0f) && (theSpeedProfile.dV1 == 0.0f) && (theSpeedProfile.s1 == 0.0f) && (theSpeedProfile.s2 == 0.0f) && (theSpeedProfile.s3 == 0.0f) && (theSpeedProfile.length == 0.0f) && (theSpeedProfile.d1d3 == 0.0f) && (theSpeedProfile.d2 == 0.0f) && (theSpeedProfile.duration == 0.0f), "constructor initialization, inputs copied, outputs initialized to 0.0f");
        theSpeedProfile.calculate(MotionSpeedProfileOrder::secondOrder);																				// calculate all other parameters
        // Are the 'input' members still their initial value after calculation ?
        theTest.assert(11, (theSpeedProfile.vStart == 1.0f) && (theSpeedProfile.vEnd == 3.0f) && (theSpeedProfile.aMax == 1.0f) && (theSpeedProfile.dMax == -1.0f) && (theSpeedProfile.jMax == 1.0f), "inputs unchanged after calculation");
        // Are the 'output' members changed from their initial 0.0f value after calculation ?
        theTest.assert(12, (theSpeedProfile.dV1 != 0.0f) && (theSpeedProfile.s1 != 0.0f) && (theSpeedProfile.s2 != 0.0f) && (theSpeedProfile.s3 != 0.0f) && (theSpeedProfile.length != 0.0f) && (theSpeedProfile.d1d3 != 0.0f) && (theSpeedProfile.d2 != 0.0f) && (theSpeedProfile.duration != 0.0f), "outputs changed from init values after calculation");
        //theSpeedProfile.print();
        }
        {
        SpeedProfileAccDec theSpeedProfile = SpeedProfileAccDec(0.0f, 2.0f, 1.0f, -1.0f, 1.0f);				// constructing a simple speedProfile with certain parameters
        theSpeedProfile.calculate(MotionSpeedProfileOrder::firstOrder);										// calculate all other parameters
        //theSpeedProfile.print();
        theTest.assert(20, (theSpeedProfile.dV1 == 0.0f) && (theSpeedProfile.d1d3 == 0.0f) && (theSpeedProfile.s1 == 0.0f) && (theSpeedProfile.s3 == 0.0f), "calculation results : firstOrder profile has dV1, s1, s3, d1d3 all equal to 0.0f");
        theTest.assert(21, (theSpeedProfile.s2 == 2.0f) && (theSpeedProfile.length == 2.0f) && (theSpeedProfile.d2 == 2.0f) && (theSpeedProfile.duration == 2.0f), "calculation results : firstOrder profile, acceleration");
        }
        {
        SpeedProfileAccDec theSpeedProfile = SpeedProfileAccDec(2.0f, 0.0f, 1.0f, -1.0f, 1.0f);				// constructing a speedProfile with certain parameters : deceleration
        theSpeedProfile.calculate(MotionSpeedProfileOrder::firstOrder);										// calculate all other parameters
        //theSpeedProfile.print();
        theTest.assert(22, (theSpeedProfile.s2 == 2.0f) && (theSpeedProfile.length == 2.0f) && (theSpeedProfile.d2 == 2.0f) && (theSpeedProfile.duration == 2.0f), "calculation results : firstOrder profile, deceleration");
        }
        {
        SpeedProfileAccDec theSpeedProfile = SpeedProfileAccDec(0.0f, 2.0f, 1.0f, -1.0f, 1.0f);				// constructing a simple speedProfile with certain parameters
        theSpeedProfile.calculate(MotionSpeedProfileOrder::secondOrder);									// calculate all other parameters
        //theSpeedProfile.print();
        //theSpeedProfile.plot('a', 0.0f, theSpeedProfile.duration, 120, theSpeedProfile.dMax, theSpeedProfile.aMax, 40);
        theTest.assert(30, (theSpeedProfile.dV1 == 0.5f) && (theSpeedProfile.d1d3 == 1.0f) && (theSpeedProfile.s1 == (1.0f / 6.0f)) && (theSpeedProfile.s3 == (2.0f - (1.0f / 6.0f))) && (theSpeedProfile.s2 == 1.0f) && (theSpeedProfile.length == 3.0f) && (theSpeedProfile.d2 == 1.0f) && (theSpeedProfile.duration == 3.0f), "calculation results : secondOrder profile acceleration");
        }
        {
        SpeedProfileAccDec theSpeedProfile = SpeedProfileAccDec(2.0f, 0.0f, 1.0f, -1.0f, 1.0f);				// constructing a speedProfile with certain parameters : deceleration
        theSpeedProfile.calculate(MotionSpeedProfileOrder::secondOrder);									// calculate all other parameters
        //theSpeedProfile.print();
        theTest.assert(31, (theSpeedProfile.dV1 == -0.5f) && (theSpeedProfile.d1d3 == 1.0f) && (theSpeedProfile.s3 == (1.0f / 6.0f)) && (theSpeedProfile.s1 == (2.0f - (1.0f / 6.0f))) && (theSpeedProfile.s2 == 1.0f) && (theSpeedProfile.length == 3.0f) && (theSpeedProfile.d2 == 1.0f) && (theSpeedProfile.duration == 3.0f), "calculation results : secondOrder profile deceleration");
        }
        {
        SpeedProfileAccDec theSpeedProfile = SpeedProfileAccDec(0.0f, 2.0f, 1.0f, -1.0f, 1.0f);				// constructing a simple speedProfile with certain parameters
        theSpeedProfile.calculate(MotionSpeedProfileOrder::secondOrder);									// calculate all other parameters
        //theSpeedProfile.print();
        // Validating a(t) at the boundaries of every phase in the speedprofile
        theTest.assert(40, (theSpeedProfile.a(0.0f) == 0.0f) && (theSpeedProfile.a(theSpeedProfile.duration) == 0.0f) && (theSpeedProfile.a(theSpeedProfile.d1d3) == 1.0f) && (theSpeedProfile.a(theSpeedProfile.d1d3 + theSpeedProfile.d2) == 1.0f), "a(t)");
        //theSpeedProfile.plot('a', 0.0f, theSpeedProfile.duration, 120, theSpeedProfile.dMax, theSpeedProfile.aMax, 40);
        // Validating v(t) at the boundaries of every phase in the speedprofile
        theTest.assert(41, (theSpeedProfile.v(0.0f) == 0.0f) && (theSpeedProfile.v(theSpeedProfile.duration) == 2.0f) && (theSpeedProfile.v(theSpeedProfile.d1d3) == 0.5f) && (theSpeedProfile.v(theSpeedProfile.d1d3 + theSpeedProfile.d2) == 1.5f), "v(t)");
        //theSpeedProfile.plot('v', 0.0f, theSpeedProfile.duration, 120, theSpeedProfile.vStart, theSpeedProfile.vEnd, 40);
        // Validating s(t) at the boundaries of every phase in the speedprofile
        theTest.assert(42, (theSpeedProfile.s(0.0f) == 0.0f) && (theSpeedProfile.s(theSpeedProfile.duration) == 3.0f) && (theSpeedProfile.s(theSpeedProfile.d1d3) == (1.0f / 6.0f)) && (abs((theSpeedProfile.s(theSpeedProfile.d1d3 + theSpeedProfile.d2) - (1.0f + (1.0f / 6.0f)))) <= 0.0000002f), "s(t)");
        //theSpeedProfile.plot('s', 0.0f, theSpeedProfile.duration, 120, 0.0f, theSpeedProfile.length, 40);
        }
        {
        SpeedProfileAccDec theSpeedProfile = SpeedProfileAccDec(2.0f, 0.0f, 1.0f, -1.0f, 1.0f);				// constructing a speedProfile with certain parameters : deceleration
        theSpeedProfile.calculate(MotionSpeedProfileOrder::secondOrder);									// calculate all other parameters
        //theSpeedProfile.print();
        // Validating a(t) at the boundaries of every phase in the speedprofile
        theTest.assert(50, (theSpeedProfile.a(0.0f) == 0.0f) && (theSpeedProfile.a(theSpeedProfile.duration) == 0.0f) && (theSpeedProfile.a(theSpeedProfile.d1d3) == -1.0f) && (theSpeedProfile.a(theSpeedProfile.d1d3 + theSpeedProfile.d2) == -1.0f), "a(t)");
        //theSpeedProfile.plot('a', 0.0f, theSpeedProfile.duration, 120, theSpeedProfile.dMax, theSpeedProfile.aMax, 40);
        // Validating v(t) at the boundaries of every phase in the speedprofile
        theTest.assert(51, (theSpeedProfile.v(0.0f) == 2.0f) && (theSpeedProfile.v(theSpeedProfile.duration) == 0.0f) && (theSpeedProfile.v(theSpeedProfile.d1d3) == 1.5f) && (theSpeedProfile.v(theSpeedProfile.d1d3 + theSpeedProfile.d2) == 0.5f), "v(t)");
        //theSpeedProfile.plot('v', 0.0f, theSpeedProfile.duration, 120, theSpeedProfile.vStart, theSpeedProfile.vEnd, 40);
        // Validating s(t) at the boundaries of every phase in the speedprofile
        theTest.assert(52, (theSpeedProfile.s(0.0f) == 0.0f) && (theSpeedProfile.s(theSpeedProfile.duration) == 3.0f) && (theSpeedProfile.s(theSpeedProfile.d1d3) == (2.0f - (1.0f / 6.0f))), "s(t)");
        //theSpeedProfile.plot('s', 0.0f, theSpeedProfile.duration, 120, 0.0f, theSpeedProfile.length, 40);
        }

    theTest.section("MotionItem Unit Tests");
        {
		// Create the necessary object instances
		// Here they are created on the stack, but I could move them to the globals if needed.
        MotionItem theMotionItem;																	// construct a motionITEM
        theTest.printValue(static_cast<int32_t>(sizeof(theMotionItem)), "memory footprint of MotionItem [bytes]");
		gCode theParser;
		theTest.printValue(static_cast<int32_t>(sizeof(theParser)), "memory footprint of gCode Parser [bytes]");
		gCodeParserResult result;
		theTest.printValue(static_cast<int32_t>(sizeof(result)), "memory footprint of gCodeParserResult [bytes]");
		MachineProperties theMachineProperties;
		theTest.printValue(static_cast<int32_t>(sizeof(theMachineProperties)), "memory footprint of theMachineProperties [bytes]");

		theParser.initialize();								// initialize the gCode parser
		theParser.getBlock("F1000 G0 Y100 ");						// load 1 block of gCode
		theParser.parseBlock(result);						// parse it, into gCodeParserResult
		theParser.getBlock("G2 X0 Y-100 J-100");				// load 1 block of gCode
		theParser.parseBlock(result);						// parse it, into gCodeParserResult
		theMotionItem.set(result, theMachineProperties);	// set properties of motionItem according to gCodeParserResult

		theMotionItem.print();

		// now validate the properties of the resulting motionItem
        }
    theTest.end();
    }

void loop()
    {
    }

