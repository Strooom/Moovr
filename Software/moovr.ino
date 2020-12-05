// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <Arduino.h>
#include "machineproperties.h"
#include "digitalInputs.h"
#include "hardwareTimers.h"
#include "logging.h"
#include "stepper.h"
#include "stepperMotorOutputs.h"
#include "version.h"

uLog theLog;
version theVersion(0, 0, 1);
inputs theHardwareInputs;
debouncedInput stop(theHardwareInputs, 0, Event::emergencyStopButtonPressed, Event::emergencyStopButtonReleased);
stepperMotorOutputs theStepperMotorOutputs;
stepBuffer theStepBuffer;
hardwareTimers theHWtimers;

void setup() {
    Serial.begin(115200);
    Serial.flush();
    theLog.setOutputIsAvailable(true);
    theLog.flush();
}

void loop() {
}

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6 Interrupt Handlers
void uart0_status_isr(void) {                               //
    //theMainController.serialInterrupt();                    // handle Serial event interrupts
}

void pit1_isr() {
    //theMainController.pit1Interrupt();        // handle timer expire interrupts
    PIT_TFLG1 = 0x1;        // clear timer interrupt flag
}

void pit2_isr() {
    //theMainController.pit2Interrupt();        // handle timer expire interrupts
    PIT_TFLG2 = 0x1;        // clear timer interrupt flag
}
#endif
