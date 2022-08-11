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
intervalTimer sampleInputsTimer(inputSamplingInterval);
stepBuffer theStepBuffer(minStepBufferTotalTimeTicks, minStepBufferLevel);

// -------------------------------------------------
// ---    application/HW-specific components     ---
// -------------------------------------------------
// ---      define inputs : all buttons and limitswitches   ---
// -------------------------------------------------

inputs allSwitchAndButtons;

debouncedInput myInputs[nmbrInputs] = {
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

outputTimer theOutputTimer;
stepperMotorOutputs theStepperMotorOutputs;

// -------------------------------------------------
// ---      define outputs : stepper motors and peripherals   ---
// -------------------------------------------------

// hostInterfaceUart theHostInterface;

// ----------------------------------
// --- interrupt handlers         ---
// ----------------------------------

bool forward{false};

void pit1_isr() {
    theOutputTimer.reload(6000U);        // reload timer
    forward   = true;
    PIT_TFLG1 = 0x1;        // clear timer interrupt flag
}

// void pit1_isr() {
//     step theStep = theStepBuffer.read();                             // read step from the stepBuffer
//     outputTimer.reload(theStep.timeBefore - 1);        // reload timer
//     //theMotorOutputs.write(theStep.signals);                          // set output-pins
//     PIT_TFLG1 = 0x1;                                                 // clear timer interrupt flag
// }

// void uart0_status_isr(void) {        //
//     uint8_t status;
//     status = UART0_S1;                // read status register. Interrupt-flag is cleared after reading this register AND reading/writing the UART0_D register until they are empty/full
//     if (status & UART_S1_RDRF)        // Transmit ready or Receive event ?
//     {
//         theHostInterface.rxReady();        // Handle ' byte received' event
//     }
//     if (status & UART_S1_TDRE)        // Transmit ready event ?
//     {
//         theHostInterface.txReady();        // Handle 'byte sent' event
//     }
// }

// ----------------------------------
// --- main application           ---
// ----------------------------------

void setup() {
    delay(5000);        // give platformio some time to switch from upload to monitoring

    Serial.begin(115200);
    Serial.flush();
    Serial.println("Moovr T0.1 - cnc3axis");

    allSwitchAndButtons.initialize();        // setup GPIO as inputs
    allSwitchAndButtons.sample();
    for (uint32_t inputIndex = 0; inputIndex < nmbrInputs; inputIndex++) {
        myInputs[inputIndex].initialize(allSwitchAndButtons.get(inputIndex));
    }

    // read config from nvs
    theStepperMotorOutputs.initialize();
    theOutputTimer.initialize();
    theOutputTimer.enable();
}

void loop() {
    if (forward) {
        Serial.print(".");
        forward = false;
    }

    // run motionControl
    // TODO : disable/re-enable timerInterrupt PITx around adding step to stepBuffer to make this threadsafe
    // This is a cooperation between motionCtrl and stepBuffer which could be at the main application level. So maybe not put it in either of those classes but just in main()
    // void motionCtrl::run() {
    //     while (theStepBuffer.needsFilling()) {
    //         step aStep = calculateStepperSignals();           // get next step from Motion...
    //         theStepBuffer.write(aStep);        // ... and pump it to buffer
    //     }
    // }

    // sample inputs and send to mainControl
    if (sampleInputsTimer.expired()) {
        allSwitchAndButtons.sample();
        for (uint32_t i = 0; i < nmbrInputs; i++) {
            event anEvent = myInputs[i].getEvent(allSwitchAndButtons.get(i));
            switch (anEvent) {
                case event::emergencyStopButtonPressed:
                    theStepperMotorOutputs.enableMotors123(true);
                    break;

                case event::emergencyStopButtonReleased:
                    theStepperMotorOutputs.enableMotors123(false);
                    break;

                default:
                    break;
            }
            // if (event::none != anEvent) {
            // Serial.println(toString(anEvent));
            // theEventBuffer.pushEvent(anEvent);
        }
    }

    // poll hostinterface
    // TODO : also here deal with temporarily disabling the related interrupt to be threadsafe

    // run mainControl stateMachine
    // theMainCtrl.run();
}
