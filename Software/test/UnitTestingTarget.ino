// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

//#define UnitTesting

#include <Arduino.h>
#include <unitTest.h>
#include "version.h"
#include "digitalInputs.h"
#include "eventbuffer.h"
#include "gcode.h"
#include "hardwareTimers.h"
#include "logging.h"
#include "machineproperties.h"
#include "motionctrl.h"
#include "stepbuffer.h"
#include "stepperMotorOutputs.h"
#include "hostinterface.h"
#include "mainctrl.h"

uLog theLog(loggingLevel::Info);
version theVersion(0, 0, 4);
stepperMotorOutputs theStepperMotorOutputs;
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

unitTest theTest;
stepBuffer theStepBuffer;
machineProperties theMachineProperties;
HostInterfaceUart theHostInterface;
mainController theMainCtrl = mainController(theMachineProperties, theEventBuffer, theHostInterface, theStepBuffer);

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

// This code removes transient events from the inputs on startup.. TODO move it to a memberfunction
    theHardwareInputs.sample();        // read HW inputs
    for (uint32_t j = 0; j <= debouncedInput::debounceMaxCount; j++) {
        for (uint32_t i = 0; i < nmbrInputs; i++) {
            event anEvent = myInputs[i].getEvent();
        }
    }

    theMainCtrl.initialize();
    theLog.pushLoggingLevel(loggingLevel::Debug);
};

void loop() {
    theMainCtrl.run();
    handleInputs();
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
