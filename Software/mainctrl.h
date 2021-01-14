// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "eventbuffer.h"              // mainController contains a reference to an eventBuffer
#include "gcode.h"                    // mainController contains a GCodeParser
#include "hostinterface.h"            // mainController contains a reference to a hostInterface
#include "machineproperties.h"        // mainController contains a reference to machineProperties
#include "motionctrl.h"               // mainController contains a motionController

enum class mainStates : uint8_t {
    ready,          // the machine is in Jogging mode, it can receive a subset of gCode, some gCode context cannot be changed
    homing,         // the machine is busy doing a homing cycle - further sub-state is controlled in homingStates
    probing,        // the machine is busy doing a probing cycle - further sub-state is controlled in probingStates
    running,        // the machine is executing a gCode program
    error           // some unrecoverable error occured
};

enum class homingStates : uint8_t {
    lost,            // machine's home position is unknown : inject GCODE to search for closing the limitswitch
    ClosingZ,        // machine is busy searching for Z limitswitch to close, wait for close event
    ClosedZ,         // Z switch is closed -> stopping the motion by changing optimization strategy, wait for stop event
    OpeningZ,        // Z axis is stopped, inject GCODE to reopen limit switch, wait for open event
    OpenedZ,         // Z switch is opened -> stop the motion, wait for stop event
    ClosingY,        // Z axis is stopped -> Zero is found, proceed with next axis..
    ClosedY,         //
    OpeningY,        //
    OpenedY,         //
    ClosingX,        //
    ClosedX,         //
    OpeningX,        //
    OpenedX,         //
    Found            //  machine has found it's X position -> fully homed.
};

enum class probingStates : uint8_t {
    lost,
    Search,
    Found
};

class mainController {
  public:
    mainController(machineProperties &theMachineProperties, eventBuffer &theEventBuffer, HostInterfaceUart &theHostInterface, stepBuffer &theStepBuffer);
    void run();                                           // read all inputs, process, generate outputs and statechange in an endless loop
    void initialize();                                    //

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    machineProperties &theMachineProperties;
    eventBuffer &theEventBuffer;
    HostInterfaceUart &theHostInterface;
    stepBuffer &theStepBuffer;

    // SubComponents objects
    overrides theOverrides;
    gCode theParser;
    gCodeParserResult theResult;
    motionCtrl theMotionCtrl = motionCtrl(theEventBuffer, theMachineProperties, theOverrides, theStepBuffer);

    // StateMachines
    mainStates mainState       = mainStates::ready;          // instance to keep track of the main state of the FSM
    homingStates homingState   = homingStates::lost;         // instance keeping track of the homing sequence
    probingStates probingState = probingStates::lost;        // instance keeping track of the probing sequence

    // IO Objects
    static constexpr uint32_t length{256};        // length of the command and response buffer
    uint8_t commandLine[length]{};                // string to store message ready at host computer receiver interface
    uint8_t responseMsg[length]{};                // string to build message to report back to host computer

    // Methods
    void handleEvents();          // handles the events such as switch close/open and end-of-motion
    void handleMessages();        // handles messages from hostInterface
    void handleCommands();        // handles messages from hostInterface
    void handleGCode();           // handles messages from hostInterface
    void homing(event anEvent);
    void probing(event anEvent);
    void discovery(event anEvent);
    void gotoState(mainStates newState);                  //
    void gotoHomingState(homingStates newState);          //
    void gotoProbingState(probingStates newState);        //
    const char *toString(mainStates aState) const;        // translate state code to string
};
