// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

// -------------------------------------------------
// ---  include files                            ---
// -------------------------------------------------
// ---    generic include files                  ---
// -------------------------------------------------

#include <unity.h>

#include "machineproperties.h"
#include "version.h"
#include "eventbuffer.h"
#include "maincontroller.h"
#include "motionctrl.h"
#include "debouncedinput.h"
#include "stepbuffer.h"
#include "gcode.h"

#include "homing.h"

// #include "mcuload.h"
// #include "logging.h"

// -------------------------------------------------
// ---    application/HW-specific include files  ---
// -------------------------------------------------

// -------------------------------------------------
// --- application top-level components          ---
// -------------------------------------------------
// ---    generic componentsles                  ---
// -------------------------------------------------

version theVersion(1, 0, 0);
machineProperties theMachineProperties;
eventBuffer theEventBuffer;
mainController theMainCtrl;
motionCtrl theMotionController;
stepBuffer theStepBuffer(minStepBufferTotalTimeTicks, minStepBufferLevel);

gCode theInterpreter;

// simplifiedMotion aMotion;
// mcuLoad theMcuLoad;

// -------------------------------------------------
// --- TMP variables for dev / test              ---
// -------------------------------------------------

// -------------------------------------------------
// ---    application/HW-specific components     ---
// -------------------------------------------------
// ---      define inputs : all buttons and limitswitches   ---
// -------------------------------------------------

debouncedInput myInputs[nmbrInputs] = {
    debouncedInput(event::limitSwitchZMaxClosed, event::limitSwitchZMaxOpened),
    debouncedInput(event::limitSwitchXMaxClosed, event::limitSwitchXMaxOpened),
    debouncedInput(event::limitSwitchYMaxClosed, event::limitSwitchYMaxOpened),
    debouncedInput(event::limitSwitchYMinClosed, event::limitSwitchYMinOpened),
    debouncedInput(event::limitSwitchZMinClosed, event::limitSwitchZMinOpened),
    debouncedInput(event::limitSwitchXMinClosed, event::limitSwitchXMinOpened),
    debouncedInput(event::emergencyStopButtonPressed, event::emergencyStopButtonReleased),
    debouncedInput(event::feedHoldResumeButtonPressed, event::feedHoldResumeButtonReleased),
    debouncedInput(event::probeSwitchClosed, event::probeSwitchOpened)};

// -------------------------------------------------
// ---      define outputs : stepper motors and peripherals   ---
// -------------------------------------------------

// -------------------------------------------------
// ---      define outputs : stepper motors and peripherals   ---
// -------------------------------------------------

// ----------------------------------
// --- interrupt handlers         ---
// ----------------------------------

// ----------------------------------
// --- development temp code      ---
// ----------------------------------

homingController theHomer;

// ----------------------------------
// --- main application           ---
// ----------------------------------

void setUp(void) {}           // before test
void tearDown(void) {}        // after test

void setup() {
    theEventBuffer.initialize();
    theMotionController.initialize(maxTicksSinceLastOutput, minStepPulseWidth);
    theHomer.start();
}

uint32_t loopCount{0};

void loop() {
    step theStep = theStepBuffer.read();        // read step from the stepBuffer, this mimics the interrupt driven output of steps
    loopCount++;
    if (loopCount == 100) {
        theEventBuffer.pushEvent(event::limitSwitchZMaxOpened);
    }

    while (theStepBuffer.needsFilling()) {
        step aStep = theMotionController.calcNextStepperMotorSignals();        // get next step from Motion...
        theStepBuffer.write(aStep);                                            // ... and pump it to buffer
    }

    if (theEventBuffer.hasEvents()) {
        event anEvent = theEventBuffer.popEvent();
        theHomer.handleEvents(anEvent);
        if (event::homingCompleted == anEvent) {
        }
    }
}

int main(int argc, char **argv) {
   UNITY_BEGIN();
    setup();
    while (true) {
        loop();
    }
    UNITY_END();
}
