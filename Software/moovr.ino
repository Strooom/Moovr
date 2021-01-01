// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <Arduino.h>
#include "logging.h"
#include "version.h"
#include "machineproperties.h"
#include "hardwaretimers.h"
#include "digitalinputs.h"
#include "eventbuffer.h"
#include "stepbuffer.h"
#include "steppermotoroutputs.h"
#include "maincontroller.h"

uLog theLog;
version theVersion(0, 0, 2);
machineProperties theMachineProperties;
hardwareTimers theHWtimers;

inputs theHardwareInputs;
enum class buttonSwitch : uint32_t {
    limitSwitchXmin,
    limitSwitchXmax,
    limitSwitchYmin,
    limitSwitchYmax,
    limitSwitchZmin,
    limitSwitchZmax,
    buttonEmergencyStop,
    buttonFeedHoldResume,
    switchProbeZ,
    nmbrButtonSwitches
};
static constexpr uint32_t nmbrInputs = 9U;
debouncedInput myInputs[nmbrInputs]  = {
    debouncedInput(theHardwareInputs, 0U, event::limitSwitchXMinClosed, event::limitSwitchXMinOpened),
    debouncedInput(theHardwareInputs, 1U, event::limitSwitchXMaxClosed, event::limitSwitchXMaxOpened),
    debouncedInput(theHardwareInputs, 2U, event::limitSwitchYMinClosed, event::limitSwitchYMinOpened),
    debouncedInput(theHardwareInputs, 3U, event::limitSwitchYMaxClosed, event::limitSwitchYMaxOpened),
    debouncedInput(theHardwareInputs, 4U, event::limitSwitchZMinClosed, event::limitSwitchZMinOpened),
    debouncedInput(theHardwareInputs, 5U, event::limitSwitchZMaxClosed, event::limitSwitchZMaxOpened),
    debouncedInput(theHardwareInputs, 6U, event::emergencyStopButtonPressed, event::emergencyStopButtonReleased),
    debouncedInput(theHardwareInputs, 7U, event::feedHoldResumeButtonPressed, event::feedHoldResumeButtonReleased),
    debouncedInput(theHardwareInputs, 8U, event::probeSwitchClosed, event::probeSwitchOpened)};
eventBuffer theEventBuffer;
void handleInputs() {
    if (theHardwareInputs.isReady()) {
        for (uint32_t i = 0; i < nmbrInputs; i++) {
            event anEvent = myInputs[i].getEvent();
            if (event::none != anEvent) {
                theEventBuffer.pushEvent(anEvent);
            }
        }
    }
}

stepperMotorOutputs theStepperMotorOutputs;
stepBuffer theStepBuffer;

HostInterfaceUart theHostInterface;

mainController theMainCtrl = mainController(theMachineProperties, theEventBuffer, theHostInterface, theStepBuffer);

void setup() {
    Serial.begin(115200);
    Serial.flush();
    theLog.setOutputIsAvailable(true);
    theLog.flush();
    theLog.output(loggingLevel::Info, "Moovr V0.2 - https://github.com/Strooom/Moovr");
    theHWtimers.enableInputTimer(true);
    theHWtimers.enableOutputTimer(true);
}

void loop() {
    handleInputs();
    theMainCtrl.run();
}

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6 Interrupt Handlers
void uart0_status_isr(void) {                               //
    uint8_t status;
    status = UART0_S1;                // read status register. Interrupt-flag is cleared after reading this register AND reading/writing the UART0_D register until they are empty/full
    if (status & UART_S1_RDRF)        // Transmit ready or Receive event ?
    {
        //        theHostInterface.rxReady();        // Handle ' byte received' event
    }
    if (status & UART_S1_TDRE)        // Transmit ready event ?
    {
        //        theHostInterface.txReady();        // Handle 'byte sent' event
    }
}

void pit1_isr() {
    step theStep = theStepBuffer.read();                             // read step from the stepBuffer
    theHWtimers.setOutputTimerReload(theStep.timeBefore - 1);        // reload timer
    theStepperMotorOutputs.write(theStep.signals);                   // set output-pins
    PIT_TFLG1 = 0x1;                                                 // clear timer interrupt flag
}

void pit2_isr() {
    theHardwareInputs.sample();        // read HW inputs
    PIT_TFLG2 = 0x1;                   // clear timer interrupt flag
}
#endif
