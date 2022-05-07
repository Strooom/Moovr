// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################


#include <Arduino.h>
#include <UnitTest.h>
#include "input.h"
#include "event.h"

UnitTest theTest;
EventBuffer theEventBuffer;
Input theInputs = Input(theEventBuffer);


void pit2_isr()
    {
    PIT_TFLG2 = 0x1;						// clear timer interrupt flag
    theInputs.run();
    }

void setup()
    {
    delay(50);
    Serial.begin(115200);					// Initializes the Serial-over-USB, which we can use for printing debug/test information
    delay(50);

    pinMode(30, OUTPUT);					// We've connected output 30 (EnableMotors456) to input Limit1
    //digitalWrite(30, HIGH);					// Setting the output high, pulls down the pulled-up output and this does not drive the optocoupler. This results in a High input due to the inputs pullup-resistor
    //delay(1);								// it takes a short time for the optocoupler to transmit the signal..
    }

void loop()
    {
    theTest.start("Input Debouncing");

    digitalWrite(30, LOW);
    delay(1);
    theInputs.initialize();
    theTest.assert(10, theInputs.switchesAndButtons[(uint8_t) limitSwitchButton::xMin].getState() == true, "reading active input");

    digitalWrite(30, HIGH);					// Setting the output high, pulls down the pulled-up output and this does not drive the optocoupler. This results in a High input due to the inputs pullup-resistor
    delay(1);
    theInputs.initialize();
    theTest.assert(11, theInputs.switchesAndButtons[(uint8_t) limitSwitchButton::xMin].getState() == false, "reading inactive input");

    theTest.assert(20, theEventBuffer.hasEvents() == false, "no events after initialization");

    theInputs.start();
    delay(50);
    theTest.assert(30, theEventBuffer.hasEvents() == false, "no events after scanning start");

    digitalWrite(30, LOW);
    delay(50);
    theTest.assert(40, theEventBuffer.hasEvents() == true, "some events detected");
    theTest.assert(41, theEventBuffer.popEvent() == Event::limitSwitchXMinClosed, "limitSwitchXMinClosed");
    theTest.assert(42, theEventBuffer.hasEvents() == false, "no more events detected");
    digitalWrite(30, HIGH);
    delay(50);
    theTest.assert(43, theEventBuffer.hasEvents() == true, "some events detected");
    theTest.assert(44, theEventBuffer.popEvent() == Event::limitSwitchXMinOpened, "limitSwitchXMinOpened");
    theTest.assert(45, theEventBuffer.hasEvents() == false, "no more events detected");

    theInputs.stop();
    theTest.end();

    theTest.start("Open / Close limitSwitch YMin");
    theInputs.initialize();
    theInputs.start();
    theTest.startTimeOut(10000);
    while (!theTest.isTimedOut())
        {
        if (theEventBuffer.hasEvents())
            {
            switch(theEventBuffer.popEvent())
                {
                case Event::none:
					Serial.println("none");
                    break;
                case Event::emergencyStopButtonPressed:
                    break;
                case Event::emergencyStopButtonReleased:
                    break;
                case Event::feedHoldResumeButtonPressed:
                    break;
                case Event::feedHoldResumeButtonReleased:
                    break;
                case Event::probeSwitchClosed:
                    break;
                case Event::probeSwitchOpenend:
                    break;
                case Event::limitSwitchXMinClosed:
                    break;
                case Event::limitSwitchYMinClosed:
					Serial.println("limitSwitchYMinClosed");
                    break;
                case Event::limitSwitchZMinClosed:
                    break;
                case Event::limitSwitchXMaxClosed:
                    break;
                case Event::limitSwitchYMaxClosed:
                    break;
                case Event::limitSwitchZMaxClosed:
                    break;
                case Event::limitSwitchXMinOpened:
                    break;
                case Event::limitSwitchYMinOpened:
					Serial.println("limitSwitchYMinOpened");
                    break;
                case Event::limitSwitchZMinOpened:
                    break;
                case Event::limitSwitchXMaxOpened:
                    break;
                case Event::limitSwitchYMaxOpened:
                    break;
                case Event::limitSwitchZMaxOpened:
                    break;
                case Event::motionAdded:
                    break;
                case Event::motionCompleted:
                    break;
                case Event::allMotionsCompleted:
                    break;
                default:
                    break;
                }
            }
        }
    theInputs.stop();
    theTest.end();


    delay(3000);
    }



