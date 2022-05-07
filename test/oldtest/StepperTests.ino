// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <Arduino.h>
#include <UnitTest.h>
#include "stepper.h"

UnitTest theTest;
Stepper theStepper;

void pit1_isr()
    {
    PIT_TFLG1 = 0x1;						// Clear timer PIT1 interrupt flag
    theStepper.output();
    }

void setup()
    {
    delay(50);
    Serial.begin(115200);					// Initializes the Serial-over-USB, which we can use for printing debug/test information
    delay(50);
    }

void loop()
    {
    theTest.start("Stepper");

    theStepper.initialize();

    theTest.assert(10, (theStepper.getLastError() == stepperError::none), "lastError == none");
    theTest.assert(11, (theStepper.stepBufferLevel == 4), "bufferLevel == 4");
    theTest.assert(12, (theStepper.stepBufferTotalTime >= theStepper.minStepBufferTotalTime), "bufferTotalTime >= 10 ms");

    theStepper.output();
    theTest.assert(20, (theStepper.stepBufferLevel == 3), "bufferLevel == 3");
    theTest.assert(21, (theStepper.stepBufferTotalTime < theStepper.minStepBufferTotalTime), "bufferTotalTime < 10 ms");

    theStepper.fill();
    theTest.assert(30, (theStepper.stepBufferLevel == 4), "bufferLevel == 4");
    theTest.assert(31, (theStepper.stepBufferTotalTime >= theStepper.minStepBufferTotalTime), "bufferTotalTime >= 10 ms");

    theStepper.fill();		// try to fill with more.. it should not..
    theTest.assert(40, (theStepper.stepBufferLevel == 4), "bufferLevel == 4");
    theTest.assert(41, (theStepper.stepBufferTotalTime >= theStepper.minStepBufferTotalTime), "bufferTotalTime >= 10 ms");

    theStepper.output();	// do 2 outputs.. this should trigger an underrun
    theStepper.output();
    theTest.assert(50, (theStepper.stepBufferLevel == 3), "bufferLevel == 3");
    theTest.assert(51, (theStepper.stepBufferTotalTime < theStepper.minStepBufferTotalTime), "bufferTotalTime < 10 ms");
    theTest.assert(52, (theStepper.getLastError() == stepperError::bufferUnderRun1), "lastError == bufferUnderRun1");

    theStepper.fill();
    theTest.assert(60, (theStepper.stepBufferLevel == 4), "bufferLevel == 4");
	//theTest.printValue(theStepper.stepBufferLevel, "stepBufferLevel");
    theTest.assert(61, (theStepper.stepBufferTotalTime >= theStepper.minStepBufferTotalTime), "bufferTotalTime >= 10 ms");
    theTest.assert(62, (theStepper.getLastError() == stepperError::none), "lastError == none");

    theTest.startTimeOut(10000);
    theStepper.start();	// switch on the HW timers..
    while (!theTest.isTimedOut())
        {
        theStepper.fill();
        }
    theStepper.stop();	// switch off the HW timers..

	theTest.end();
	delay(1000);
    }
