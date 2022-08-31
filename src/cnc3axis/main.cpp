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
#include "mcuload.h"
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

version theVersion(0, 0, 9);
machineProperties theMachineProperties;
eventBuffer theEventBuffer;
mainController theMainCtrl;
motionCtrl theMotionController;
intervalTimer sampleInputsTimer(inputSamplingInterval);
stepBuffer theStepBuffer(minStepBufferTotalTimeTicks, minStepBufferLevel);
gCode theParser;
simplifiedMotion aMotion;
mcuLoad theMcuLoad;

intervalTimer reportMcuLoad;
uint32_t nmbrRuns{0};

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

    theMotionController.initialize(maxTicksSinceLastOutput, minStepPulseWidth);
    theStepperMotorOutputs.initialize();
    theStepperMotorOutputs.enableMotors123(true);
    theOutputTimer.initialize();
    theOutputTimer.enable();

    // theParser.getBlockFromString("G1 X100 F3600");
    // theParser.parseBlock(aMotion);
    // theMotionController.append(aMotion);
    // theParser.getBlockFromString("X0");
    // theParser.parseBlock(aMotion);
    // theMotionController.append(aMotion);
    // theParser.getBlockFromString("X100 Y50 Z25");
    // theParser.parseBlock(aMotion);
    // theMotionController.append(aMotion);
    // theParser.getBlockFromString("X0Y0Z0");
    // theParser.parseBlock(aMotion);
    // theMotionController.append(aMotion);
    // theMotionController.start();

    theParser.getBlockFromString("G0 X300");
    theParser.parseBlock(aMotion);
    theMotionController.append(aMotion);
    theParser.getBlockFromString("G0 Y160");
    theParser.parseBlock(aMotion);
    theMotionController.append(aMotion);
    theParser.getBlockFromString("G0 Z60");
    theParser.parseBlock(aMotion);
    theMotionController.append(aMotion);

    theParser.getBlockFromString("G0 X0Y0Z0");
    theParser.parseBlock(aMotion);
    theMotionController.append(aMotion);


    // theParser.getBlockFromString("G2 X-50 I-50 F1500");
    // theParser.parseBlock(aMotion);
    // theMotionController.append(aMotion);
    // theParser.getBlockFromString("G3 X50 I50 F1500");
    // theParser.parseBlock(aMotion);
    // theMotionController.append(aMotion);
    // theParser.getBlockFromString("G1 X0 F1500");
    // theParser.parseBlock(aMotion);
    // theMotionController.append(aMotion);

    // theParser.getBlockFromString("G1 Y50");
    // theParser.parseBlock(aMotion);
    // theMotionController.append(aMotion);
    // theParser.getBlockFromString("G1 Z10");
    // theParser.parseBlock(aMotion);
    // theMotionController.append(aMotion);
    // theParser.getBlockFromString("G1 X0Y0Z0");
    // theParser.parseBlock(aMotion);
    // theMotionController.append(aMotion);

    theMotionController.start();

    reportMcuLoad.start(500U);
    theMcuLoad.start();
}

void loop() {
    // run motionControl
    while (theStepBuffer.needsFilling()) {
        step aStep = theMotionController.calcNextStepperMotorSignals();        // get next step from Motion...
        NVIC_DISABLE_IRQ(IRQ_PIT_CH1);                                         // TODO disable timer interrupts to make this threadsafe
        theStepBuffer.write(aStep);                                            // ... and pump it to buffer
        NVIC_ENABLE_IRQ(IRQ_PIT_CH1);                                          // TODO re-enable timer interrupts
    }

    if (theEventBuffer.hasEvents()) {
        event anEvent = theEventBuffer.popEvent();
        Serial.println(toString(anEvent));
        if (event::allMotionsCompleted == anEvent) {
            Serial.print("Max load : ");
            Serial.print(theMcuLoad.getMaxLoad());
            Serial.println(" %");

            // if (nmbrRuns > 0) {
            //     --nmbrRuns;

            //     theParser.getBlockFromString("G0 X300");
            //     theParser.parseBlock(aMotion);
            //     theMotionController.append(aMotion);
            //     theParser.getBlockFromString("G0 Y160");
            //     theParser.parseBlock(aMotion);
            //     theMotionController.append(aMotion);
            //     theParser.getBlockFromString("G0 Z60");
            //     theParser.parseBlock(aMotion);
            //     theMotionController.append(aMotion);

            //     theParser.getBlockFromString("G0 X0Y0Z0");
            //     theParser.parseBlock(aMotion);
            //     theMotionController.append(aMotion);

            //     theMotionController.start();
            // }
        }
    }

    theMcuLoad.run();

    if (reportMcuLoad.expired()) {
        Serial.print("MCU load : ");
        Serial.print(theMcuLoad.getLoad());
        Serial.println(" %");
    }

    // sample inputs and send to mainControl
    // if (sampleInputsTimer.expired()) {
    //     allSwitchAndButtons.sample();
    //     for (uint32_t i = 0; i < nmbrInputs; i++) {
    //         event anEvent = myInputs[i].getEvent(allSwitchAndButtons.get(i));
    //         switch (anEvent) {
    //             case event::emergencyStopButtonPressed:
    //                 theStepperMotorOutputs.enableMotors123(true);
    //                 break;

    //             case event::emergencyStopButtonReleased:
    //                 theStepperMotorOutputs.enableMotors123(false);
    //                 break;

    //             default:
    //                 break;
    //         }
    //         // if (event::none != anEvent) {
    //         // Serial.println(toString(anEvent));
    //         // theEventBuffer.pushEvent(anEvent);
    //     }
    // }

    // poll hostinterface
    // TODO : also here deal with temporarily disabling the related interrupt to be threadsafe

    // run mainControl stateMachine
    // theMainCtrl.run();
}
