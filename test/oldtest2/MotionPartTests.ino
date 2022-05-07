// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <Arduino.h>
#include <UnitTest.h>
#include "motionpart.h"

UnitTest theTest;

void setup()
    {
    theTest.init();
    theTest.start("MotionPart Unit Tests");
        {
        SpeedProfileAccDec theMotionPart = SpeedProfileAccDec(3.0f, 7.0f, 2.0f, -2.0f, 1.0f);	// constructing a motionpart with certain boundaries
        theTest.printValue(sizeof(theMotionPart), "memory footprint of MotionPart [bytes]");
        theMotionPart.calculate(MotionSpeedProfileOrder::secondOrder);																				// calculate all other parameters
        theMotionPart.print();
		theMotionPart.plot('a', 0.0f, theMotionPart.duration, 120, theMotionPart.dMax, theMotionPart.aMax, 40);
		theMotionPart.plot('v', 0.0f, theMotionPart.duration, 120, theMotionPart.vStart, theMotionPart.vEnd, 40);
		theMotionPart.plot('s', 0.0f, theMotionPart.duration, 120, 0.0f, theMotionPart.length, 40);
        }

    // First Order Profile testing
        {
        SpeedProfileAccDec theMotionPart = SpeedProfileAccDec(1.1f, 2.2f, 3.3f, 4.4f, 5.5f);	// constructing a motionpart with certain boundaries
        theTest.assert(10, (theMotionPart.vStart == 1.1f) && (theMotionPart.vEnd == 2.2f) && (theMotionPart.aMax == 3.3f) && (theMotionPart.dMax == 4.4f) && (theMotionPart.jMax == 5.5f) && (theMotionPart.s1 == 0.0f) && (theMotionPart.s2 == 0.0f) && (theMotionPart.s3 == 0.0f) && (theMotionPart.length == 0.0f) && (theMotionPart.d1d3 == 0.0f) && (theMotionPart.d2 == 0.0f), "constructor initialization");
        }
        {
        SpeedProfileAccDec theMotionPart = SpeedProfileAccDec(0.0f, 1.0f, 1.0f, -1.0f, 10.0f);	// constructing a motionpart with certain parameters
        theMotionPart.calculate(MotionSpeedProfileOrder::firstOrder);																				// calculate all other parameters
        theTest.assert(20, (theMotionPart.d1d3 == 0.0f) && (theMotionPart.s1 == 0.0f) && (theMotionPart.s3 == 0.0f), "calculation results : firstOrder profile has no s1, s3, d1d3");
        theTest.assert(21, (theMotionPart.s2 == 0.5f) && (theMotionPart.d2 == 1.0f) && (theMotionPart.length == 0.5f), "calculation results : firstOrder profile, acceleration");
        }
        {
        SpeedProfileAccDec theMotionPart = SpeedProfileAccDec(1.0f, 0.0f, 1.0f, -1.0f, 10.0f);	// constructing a motionpart with certain parameters : deceleration
        theMotionPart.calculate(MotionSpeedProfileOrder::firstOrder);																				// calculate all other parameters
        theTest.assert(22, (theMotionPart.s2 == 0.5f) && (theMotionPart.d2 == 1.0f) && (theMotionPart.length == 0.5f), "calculation results : firstOrder profile, deceleration");
        }
        {
        SpeedProfileAccDec theMotionPart = SpeedProfileAccDec(10.0f, 12.0f, 1.0f, -10.0f, 10.0f);	// constructing a motionpart with certain parameters
        theMotionPart.calculate(MotionSpeedProfileOrder::firstOrder);																				// calculate all other parameters
        theTest.assert(23, (theMotionPart.s2 == 22.0f) && (theMotionPart.d2 == 2.0f) && (theMotionPart.length == 22.0f), "calculation results : firstOrder profile, acceleration");
        }
        {
        SpeedProfileAccDec theMotionPart = SpeedProfileAccDec(10.0f, 8.0f, 1.0f, -0.1f, 10.0f);	// constructing a motionpart with certain parameters : deceleration
        theMotionPart.calculate(MotionSpeedProfileOrder::firstOrder);																				// calculate all other parameters
        theTest.assert(24, (theMotionPart.s2 == 180.0f) && (theMotionPart.d2 == 20.0f) && (theMotionPart.length == 180.0f), "calculation results : firstOrder profile, deceleration");
        }


    // Second Order Profile testing
        {
        SpeedProfileAccDec theMotionPart = SpeedProfileAccDec(0.0f, 3.0f, 1.0f, -1.0f, 1.0f);	// constructing a motionpart with certain parameters
        theMotionPart.calculate(MotionSpeedProfileOrder::secondOrder);																				// calculate all other parameters
        theTest.assert(30, (theMotionPart.s1 == 1.0f / 6.0f), "calculation results : secondOrder profile");
        theTest.assert(31, (theMotionPart.s3 == (3.0f - (1.0f / 6.0f))), "calculation results : secondOrder profile");
        theTest.assert(32, (theMotionPart.d1d3 == 1.0f), "calculation results : secondOrder profile");
        theTest.assert(33, (theMotionPart.s2 == 3.0f), "calculation results : secondOrder profile");
        theTest.assert(34, (theMotionPart.d2 == 2.0f), "calculation results : secondOrder profile");
        theTest.assert(35, (theMotionPart.length == 6.0f), "calculation results : secondOrder profile");
        }

    // TODO : what happens when we call a(t), v(t) or s(t) with t outside the interval [0, duration] ?? Could amaybe happen due to float rounding errors
    // maybe we need to limit t to the valid interval...

    theTest.end();
    }

void loop()
    {
    }

