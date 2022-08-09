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

#include <Arduino.h>
#include "machineproperties.h"
#include "version.h"
#include "eventbuffer.h"
#include "maincontroller.h"
#include "motionctrl.h"
#include "debouncedinput.h"
#include "runtimer.h"
#include "stepbuffer.h"

// -------------------------------------------------
// ---    application/HW-specific include files  ---
// -------------------------------------------------

#include "switches.h"
#include "timers.h"
#include "steppermotor.h"
#include "hostinterface_uart.h"

// #include "logging.h"
// #include "overrides.h"
// #include "digitalinputs.h"

// -------------------------------------------------
// --- application top-level components          ---
// -------------------------------------------------
// ---    generic componentsles                  ---
// -------------------------------------------------

version theVersion(0, 0, 3);
machineProperties theMachineProperties;
eventBuffer theEventBuffer;
mainController theMainCtrl;
motionCtrl theMotionController;
intervalTimer sampleInputsTimer(10U);
stepBuffer theStepBuffer(100U, 4U);

// -------------------------------------------------
// ---    application/HW-specific components     ---
// -------------------------------------------------
// ---      define inputs : all buttons and limitswitches   ---
// -------------------------------------------------

inputs allSwitchAndButtons;

static constexpr uint32_t nmbrInputs = 9U;
debouncedInput myInputs[nmbrInputs]  = {
     debouncedInput(event::limitSwitchXMinClosed, event::limitSwitchXMinOpened),
     debouncedInput(event::limitSwitchXMaxClosed, event::limitSwitchXMaxOpened),
     debouncedInput(event::limitSwitchYMinClosed, event::limitSwitchYMinOpened),
     debouncedInput(event::limitSwitchYMaxClosed, event::limitSwitchYMaxOpened),
     debouncedInput(event::limitSwitchZMinClosed, event::limitSwitchZMinOpened),
     debouncedInput(event::limitSwitchZMaxClosed, event::limitSwitchZMaxOpened),
     debouncedInput(event::emergencyStopButtonPressed, event::emergencyStopButtonReleased),
     debouncedInput(event::feedHoldResumeButtonPressed, event::feedHoldResumeButtonReleased),
     debouncedInput(event::probeSwitchClosed, event::probeSwitchOpened)};

// -------------------------------------------------
// ---      define outputs : stepper motors and peripherals   ---
// -------------------------------------------------

hardwareTimers outputTimer;
stepperMotorOutputs theMotorOutputs;


// -------------------------------------------------
// ---      define outputs : stepper motors and peripherals   ---
// -------------------------------------------------

hostInterfaceUart theHostInterface;


// ----------------------------------
// --- interrupt handlers         ---
// ----------------------------------

void pit1_isr() {
    step theStep = theStepBuffer.read();                             // read step from the stepBuffer
    outputTimer.setOutputTimerReload(theStep.timeBefore - 1);        // reload timer
    //theMotorOutputs.write(theStep.signals);                          // set output-pins
    PIT_TFLG1 = 0x1;                                                 // clear timer interrupt flag
}

void uart0_status_isr(void) {        //
    uint8_t status;
    status = UART0_S1;                // read status register. Interrupt-flag is cleared after reading this register AND reading/writing the UART0_D register until they are empty/full
    if (status & UART_S1_RDRF)        // Transmit ready or Receive event ?
    {
        theHostInterface.rxReady();        // Handle ' byte received' event
    }
    if (status & UART_S1_TDRE)        // Transmit ready event ?
    {
        theHostInterface.txReady();        // Handle 'byte sent' event
    }
}

// ----------------------------------
// --- main application           ---
// ----------------------------------

void setup() {
    // read config from nvs
    theMotorOutputs.initialize();
    outputTimer.enableOutputTimer(true);        // start outputs timer
}

void loop() {
    // run motionControl
    // TODO : disable/re-enable timerInterrupt PITx around adding step to stepBuffer to make this threadsafe

    // sample inputs and send to mainControl
    if (sampleInputsTimer.expired()) {
        allSwitchAndButtons.sample();
        for (uint32_t i = 0; i < nmbrInputs; i++) {
            event anEvent = myInputs[i].getEvent(allSwitchAndButtons.get(i));
            if (event::none != anEvent) {
                theEventBuffer.pushEvent(anEvent);
            }
        }
    }

    // poll hostinterface
    // TODO : also here deal with temporarily disabling the related interrupt to be threadsafe

    // run mainControl stateMachine
    theMainCtrl.run();
}
