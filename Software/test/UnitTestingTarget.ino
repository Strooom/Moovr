// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

//#define UnitTesting

#include <Arduino.h>
#include <unitTest.h>
#include "logging.h"
#include "version.h"
#include "machineproperties.h"
//#include "digitalInputs.h"
//#include "hardwareTimers.h"
#include "eventbuffer.h"
#include "stepper.h"
//#include "stepperMotorOutputs.h"
#include "motionctrl.h"

uLog theLog;
version theVersion(0, 0, 2);

//stepperMotorOutputs theStepperMotorOutputs;
//hardwareTimers theHWtimers;
//inputs theHardwareInputs;
//debouncedInput limitSwitch1(theHardwareInputs, 0, Event::limitSwitchXMaxClosed, Event::limitSwitchXMaxOpened);
//debouncedInput button1(theHardwareInputs, 10, Event::limitSwitchXMaxClosed, Event::limitSwitchXMaxOpened);

unitTest theTest;

eventBuffer theEventBuffer;
stepBuffer theStepBuffer;
machineProperties theMachineProperties;
overrides theOverrides;

motionCtrl theMotionController(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);

void setup() {
    Serial.begin(115200);
    Serial.flush();
    theLog.setOutputIsAvailable(true);
    theLog.flush();

};

void loop() {
}

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6 Interrupt Handlers
void uart0_status_isr(void) {                               //
    //theMainController.serialInterrupt();                    // handle Serial event interrupts
}

void pit1_isr() {
    step theStep = theStepBuffer.read();        //
    PIT_LDVAL1   = theStep.timeBefore - 1;      // TODO : call member on the HW abstraction for this timer ?
    GPIOC_PDOR   = theStep.signals;             // TODO : call member on the HW abstraction for these motor outputs ?
    PIT_TFLG1    = 0x1;                         // clear timer interrupt flag
}

void pit2_isr() {
    //theHardwareInputs.sample();
    //if (Event::none != limitSwitch1.getEvent()) {
    //    theLog.output(loggingLevel::Debug, "limitSwitch event");
    //}
    //if (Event::none != button1.getEvent()) {
    //    theLog.output(loggingLevel::Debug, "button event");
    //}
    PIT_TFLG2 = 0x1;        // clear timer interrupt flag
}
#endif
