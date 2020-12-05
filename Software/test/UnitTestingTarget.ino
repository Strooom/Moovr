// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#define UnitTesting

#include <Arduino.h>
#include <unitTest.h>
#include "MCU.h"
#include "digitalInputs.h"
#include "hardwareTimers.h"
#include "logging.h"
#include "stepper.h"
#include "stepperMotorOutputs.h"
#include "version.h"

uLog theLog;
version theVersion(0, 0, 1);

stepperMotorOutputs theStepperMotorOutputs;
stepBuffer theStepBuffer;
hardwareTimers theHWtimers;
inputs theHardwareInputs;
debouncedInput limitSwitch1(theHardwareInputs, 0, Event::limitSwitchXMaxClosed, Event::limitSwitchXMaxOpened);
debouncedInput button1(theHardwareInputs, 10, Event::limitSwitchXMaxClosed, Event::limitSwitchXMaxOpened);

unitTest theTest;

uint32_t testValue = 0;

void setup() {
    Serial.begin(115200);
    Serial.flush();
    theLog.setOutputIsAvailable(true);
    theLog.flush();

    theHWtimers.enableOutputTimer(true);
};

void loop() {
    if (theStepBuffer.needsFilling()) {
        theStepBuffer.write(step{999, 3});
        theStepBuffer.write(step{1, 1});
        theStepBuffer.write(step{0, 3});
        theStepBuffer.write(step{1, 1});
        theStepBuffer.write(step{0, 3});
        theStepBuffer.write(step{1, 1});
        theStepBuffer.write(step{0, 0});
    }
}

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6 Interrupt Handlers
void uart0_status_isr(void) {                               //
    //theMainController.serialInterrupt();                    // handle Serial event interrupts
}

void pit1_isr() {
    step theStep = theStepBuffer.read();        //
    PIT_LDVAL1   = theStep.timeBefore;          // TODO : call member on the HW abstraction for this timer ?
    GPIOC_PDOR   = theStep.signals;             // TODO : call member on the HW abstraction for these motor outputs ?
    PIT_TFLG1    = 0x1;                         // clear timer interrupt flag
}

void pit2_isr() {
    theHardwareInputs.sample();
    if (Event::none != limitSwitch1.getEvent()) {
        theLog.output(loggingLevel::Debug, "limitSwitch event");
    }
    if (Event::none != button1.getEvent()) {
        theLog.output(loggingLevel::Debug, "button event");
    }
    PIT_TFLG2 = 0x1;        // clear timer interrupt flag
}
#endif
