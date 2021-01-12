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

#ifdef WIN32
const char ESC = '?';        /// ASCII Escape, all non GCode commands need ESC as first character on the line
#else
const char ESC = 0x1B;        /// ASCII Escape, all non GCode commands need ESC as first character on the line
#endif
const char CR = 0x0D;        /// ASCII Carriage Return, marks the end of the line
const char LF = 0x0A;        /// ASCII LineFeed, marks the end of the line

enum class command : uint8_t {
    doHome      = 'H',        ///< initiates a homing cycle
    doProbe     = 'P',        ///< initiates a probing
    jog         = 'J',        ///< puts the machine into jogging mode
    uploadFile  = 'U',
    executeFile = 'E',
    override    = 'O',
    getStatus   = '?',        ///< request the current machine status
    cancel      = 24,         ///< cancel command
    gCode       = 'G'         ///< this is a placeholder for anything which is NOT a command, and is in fact treated like it's cGode
};

enum class mainState : uint8_t {
    Ready,          ///< the machine is ready to accept Commands or gCode
    Homing,         ///< the machine is busy doing a homing cycle - further sub-state is controlled in homingState
    Jogging,        ///< the machine is in Jogging mode, it can receive a subset of gCode, some gCode context cannot be changed
    Probing,
    Running,        ///< the machine is executing a gCode  (either from a file stored on SD card or from manual commands)
    Pausing,
    Loading,        ///< a gCode fle is being uploaded from the Host to the SD-card of the controller
    Error           ///< some unrecoverable error occured
};

enum class homingState : uint8_t {
    Lost,            // machine's home position is unknown : inject GCODE to search for closing the limitswitch
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

enum class probingState : uint8_t {
    Lost,
    Search,
    Found
};

class mainController {
  public:
    mainController(machineProperties &theMachineProperties, eventBuffer &theEventBuffer, HostInterfaceUart &theHostInterface, stepBuffer &theStepBuffer);
    void run();        // read all inputs, process, generate outputs and statechange in an endless loop
    void initialize();

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
    mainState theMainState       = mainState::Ready;          // instance to keep track of the main state of the FSM
    homingState theHomingState   = homingState::Lost;         // instance keeping track of the homing sequence
    probingState theProbingState = probingState::Lost;        // instance keeping track of the probing sequence

    // IO Objects
    uint8_t commandLine[512]{};        // string to store message ready at host computer receiver interface
    uint8_t responseMsg[512]{};        // string to build message to report back to host computer

    // Methods
    void handleEvents();          // handles the events such as switch close/open and end-of-motion
    void handleMessages();        // handles messages from hostInterface
    void homing(event anEvent);
    void probing(event anEvent);
    void discovery(event anEvent);
    const char *toString() const;        // translate state code to string
};
