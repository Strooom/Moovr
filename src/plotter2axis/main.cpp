// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <Arduino.h>
#include "machineproperties.h"
#include "version.h"
#include "eventbuffer.h"
#include "maincontroller.h"
#include "hostinterface_uart.h"
#include "motionctrl.h"

// #include "logging.h"
// #include "overrides.h"
// #include "stepbuffer.h"
// #include "digitalinputs.h"
// #include "steppermotoroutputs.h"
// #include "hardwaretimers.h"

// -------------------------------------------------
// --- application top-level components          ---
// -------------------------------------------------

version theVersion(0, 0, 4);
machineProperties theMachineProperties;
eventBuffer theEventBuffer;
hostInterfaceUart theHostInterface;

mainController theMainCtrl;

motionCtrl theMotionController;

// hardwareTimers theHWtimers;
// stepperMotorOutputs theStepperMotorOutputs;
// stepBuffer theStepBuffer;
// inputs theHardwareInputs;

// enum class buttonSwitch : uint32_t {
//     limitSwitchXmin,
//     limitSwitchXmax,
//     limitSwitchYmin,
//     limitSwitchYmax,
//     limitSwitchZmin,
//     limitSwitchZmax,
//     buttonEmergencyStop,
//     buttonFeedHoldResume,
//     switchProbeZ,
//     nmbrButtonSwitches
// };

// constexpr uint32_t nmbrInputs       = 9U;
// debouncedInput myInputs[nmbrInputs] = {

//     debouncedInput(theHardwareInputs, 0U, event::limitSwitchXMinClosed, event::limitSwitchXMinOpened),
//     debouncedInput(theHardwareInputs, 1U, event::limitSwitchXMaxClosed, event::limitSwitchXMaxOpened),
//     debouncedInput(theHardwareInputs, 2U, event::limitSwitchYMinClosed, event::limitSwitchYMinOpened),
//     debouncedInput(theHardwareInputs, 3U, event::limitSwitchYMaxClosed, event::limitSwitchYMaxOpened),
//     debouncedInput(theHardwareInputs, 4U, event::limitSwitchZMinClosed, event::limitSwitchZMinOpened),
//     debouncedInput(theHardwareInputs, 5U, event::limitSwitchZMaxClosed, event::limitSwitchZMaxOpened),
//     debouncedInput(theHardwareInputs, 6U, event::emergencyStopButtonPressed, event::emergencyStopButtonReleased),
//     debouncedInput(theHardwareInputs, 7U, event::feedHoldResumeButtonPressed, event::feedHoldResumeButtonReleased),
//     debouncedInput(theHardwareInputs, 8U, event::probeSwitchClosed, event::probeSwitchOpened)};

// void handleInputs() {
//     if (theHardwareInputs.isReady()) {
//         for (uint32_t i = 0; i < nmbrInputs; i++) {
//             event anEvent = myInputs[i].getEvent();
//             if (event::none != anEvent) {
//                 theEventBuffer.pushEvent(anEvent);
//             }
//         }
//     }
// }

// outputs ...


// ----------------------------------
// --- main application           ---
// ----------------------------------

void setup() {
}

void loop() {
    // poll hostinterface and send to mainControl
    // run motionControl
    theMainCtrl.run();
}



// ----------------------------------
// --- interrupt handlers         ---
// ----------------------------------

// void uart0_status_isr(void) {        //

//     uint8_t status;
//     status = UART0_S1;                     // read status register. Interrupt-flag is cleared after reading this register AND reading/writing the UART0_D register until they are empty/full
//     if (status & UART_S1_RDRF) {           // Transmit ready or Receive event ?
//         theHostInterface.rxReady();        // Handle ' byte received' event
//     }                                      //
//     if (status & UART_S1_TDRE) {           // Transmit ready event ?
//         theHostInterface.txReady();        // Handle 'byte sent' event
//     }                                      //
// }

// void pit1_isr() {
//     // step theStep = theStepBuffer.read();                             // read step from the stepBuffer
//     // theHWtimers.setOutputTimerReload(theStep.timeBefore - 1);        // reload timer
//     // theStepperMotorOutputs.write(theStep.signals);                   // set output-pins
//     PIT_TFLG1 = 0x1;                                                 // clear timer interrupt flag
// }
