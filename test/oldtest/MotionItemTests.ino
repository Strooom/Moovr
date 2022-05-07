// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <Arduino.h>
#include <UnitTest.h>
#include "motionitem.h"

UnitTest theTest;

void setup()
    {
    theTest.init();
    theTest.start("MotionItem Unit Tests");
        {
        SpeedProfileAccDec motionPartLeft = SpeedProfileAccDec(0.0f, 1.0f, 1.0f, -1.0f, 10.0f);		// constructing a motionpart at beginning
        SpeedProfileAccDec motionPartRight = SpeedProfileAccDec(0.0f, 1.0f, 1.0f, -1.0f, 10.0f);	// constructing a motionpart at ending
		motionPartLeft.calculate(MotionSpeedProfileOrder::firstOrder);					// calculate all parameters
		motionPartRight.calculate(MotionSpeedProfileOrder::firstOrder);					// calculate all parameters

		MotionItem theMotionItem;													// construct a motionITEM 

		theMotionItem.fromParts(motionPartLeft, motionPartRight, 0.0f, 0.0f);		// populate the data from the two PARTS

		theTest.printValue(sizeof(theMotionItem), "memory footprint of MotionItem [bytes]");

		theTest.assert(1, theMotionItem.vMax == 0.0f, "someTest");
        }

    theTest.end();
    }

void loop()
    {
    }

