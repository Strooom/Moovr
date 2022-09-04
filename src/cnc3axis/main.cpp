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
#include "gcode.h"

#include "homing.h"

// #include "mcuload.h"
// #include "logging.h"

// -------------------------------------------------
// ---    application/HW-specific include files  ---
// -------------------------------------------------

#include "switches.h"
#include "timers.h"
#include "steppermotor.h"
#include "hostinterface_uart.h"

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
intervalTimer sampleInputsTimer;
stepBuffer theStepBuffer(minStepBufferTotalTimeTicks, minStepBufferLevel);

gCode theParser;

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

inputs allSwitchAndButtons;

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

outputTimer theOutputTimer;
stepperMotorOutputs theStepperMotorOutputs;

// -------------------------------------------------
// ---      define outputs : stepper motors and peripherals   ---
// -------------------------------------------------

// hostInterfaceUart theHostInterface;

// ----------------------------------
// --- interrupt handlers         ---
// ----------------------------------

void pit1_isr() {
    step theStep = theStepBuffer.read();                  // read step from the stepBuffer
    theOutputTimer.reload(theStep.timeBefore - 1);        // reload timer
    theStepperMotorOutputs.write(theStep.signals);        // set output-pins
    PIT_TFLG1 = 0x1;                                      // clear timer interrupt flag
}

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
// --- development temp code      ---
// ----------------------------------

bool upDown{false};

void test() {
    point currentPosition;
    simplifiedMotion aMotion;
    theMotionController.getMachinePosition(currentPosition);

    if (upDown) {
        aMotion.set(currentPosition, axis::Z, 10.0, theMachineProperties.vHoming);
        // theParser.getBlockFromString("G1 X50 F1200");
        // theParser.parseBlock(aMotion);

        upDown = false;
    } else {
        aMotion.set(currentPosition, axis::Z, -10.0, theMachineProperties.vHoming);
        // theParser.getBlockFromString("G1 X0 F1200");
        // theParser.parseBlock(aMotion);
        upDown = true;
    }
    theMotionController.append(aMotion);
    theMotionController.start();
}

intervalTimer dispStatusTimer;
void dispStatus() {
    // point currentPosition;
    // theMotionController.getMachinePosition(currentPosition);
    // Serial.printf("X:%d, Y:%d, Z:%d\n", currentPosition.inSteps[0], currentPosition.inSteps[1], currentPosition.inSteps[2]);
    Serial.printf("X:%d, Y:%d, Z:%d\n", theMotionController.machinePositionInSteps[0], theMotionController.machinePositionInSteps[1], theMotionController.machinePositionInSteps[2]);
}

homingController theHomer;

// ----------------------------------
// --- main application           ---
// ----------------------------------

void setup() {
    delay(3000);        // give platformio some time to switch from upload to monitoring

    Serial.begin(115200);
    Serial.flush();
    Serial.println("Moovr");
    Serial.println("cnc3axis");
    Serial.println(theVersion.toString());

    theEventBuffer.initialize();

    allSwitchAndButtons.initialize();        // setup GPIO as inputs
    allSwitchAndButtons.sample();
    for (uint32_t inputIndex = 0; inputIndex < nmbrInputs; inputIndex++) {
        myInputs[inputIndex].initialize(allSwitchAndButtons.get(inputIndex));
    }
    sampleInputsTimer.start(inputSamplingInterval);

    theMotionController.initialize(maxTicksSinceLastOutput, minStepPulseWidth);
    theStepperMotorOutputs.initialize();
    theStepperMotorOutputs.enableMotors123(true);
    theOutputTimer.initialize();
    theOutputTimer.enable();

    theMotionController.resetMachinePosition();

    dispStatusTimer.start(100U);

    dispStatus();
    theHomer.start();
}

void loop() {
    while (theStepBuffer.needsFilling()) {
        step aStep = theMotionController.calcNextStepperMotorSignals();        // get next step from Motion...
        NVIC_DISABLE_IRQ(IRQ_PIT_CH1);                                         // disable timer interrupts to make this threadsafe
        theStepBuffer.write(aStep);                                            // ... and pump it to buffer
        NVIC_ENABLE_IRQ(IRQ_PIT_CH1);                                          // re-enable timer interrupts
    }

    if (sampleInputsTimer.expired()) {
        allSwitchAndButtons.sample();
        for (uint32_t i = 0; i < nmbrInputs; i++) {
            event anEvent = myInputs[i].getEvent(allSwitchAndButtons.get(i));
            if (event::none != anEvent) {
                theEventBuffer.pushEvent(anEvent);
            }
        }
    }

    if (theEventBuffer.hasEvents()) {
        event anEvent = theEventBuffer.popEvent();
        Serial.println(toString(anEvent));
        theHomer.handleEvents(anEvent);
        if (event::homingCompleted == anEvent) {
            dispStatusTimer.stop();
        }
    }

    if (dispStatusTimer.expired()) {
        dispStatus();
    }
}
