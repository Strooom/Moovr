// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

//#define UnitTesting

#include <Arduino.h>                   //
#include "digitalinputs_hal.h"         //
#include "hardwaretimers_hal.h"        //
#include "hostinterface_hal.h"         //
#include "logging.h"                   //
#include "mainctrl.h"                  //
#include "stepbuffer.h"                //
#include "stepperoutputs_hal.h"        //
#include "version.h"                   //

uLog theLog(loggingLevel::Info);
version theVersion(0, 0, 4);

hardwareTimers theHWtimers;
stepperMotorOutputs theStepperMotorOutputs;
inputs theHardwareInputs;
hostInterfaceUart theHostInterface;
stepBuffer theStepBuffer;

mainController theMainCtrl = mainController(theHardwareInputs, theHostInterface, theStepBuffer);

void setup() {
    Serial.begin(115200);
    Serial.flush();
    Serial.print("\n\n\n\n");
    delay(1000);
    theLog.setOutputIsAvailable(true);
    theLog.flush();

    NVIC_SET_PRIORITY(IRQ_PIT_CH1, 10);        // giving the PIT1 a high priority, higher than all others.. to make output stepsignals timing more accurate.

    theHWtimers.enableInputTimer(true);
    theHWtimers.enableOutputTimer(true);
    theMainCtrl.initialize();
    theLog.pushLoggingLevel(loggingLevel::Debug);
};

void loop() {
    theMainCtrl.run();
}

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6 Interrupt Handlers
void uart0_status_isr(void) {                               //
    //theMainController.serialInterrupt();                    // handle Serial event interrupts
}

FASTRUN void pit1_isr() {
    step theStep = theStepBuffer.read();                             // read step from the stepBuffer
    theHWtimers.setOutputTimerReload(theStep.timeBefore - 1);        // reload timer
    theStepperMotorOutputs.write(theStep.signals);                   // set output-pins
    PIT_TFLG1 = 0x1;                                                 // clear timer interrupt flag
}

FASTRUN void pit2_isr() {
    theHardwareInputs.sample();        // read HW inputs
    PIT_TFLG2 = 0x1;                   // clear timer interrupt flag
}
#endif
