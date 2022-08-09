// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include <stdint.h>
#include "command.h"
#include "mainstate.h"
#include "homingstate.h"
#include "probingstate.h"

// #include "button.h"					// mainController contains an array of button objects
// #include "event.h"					// mainController contains an eventBuffer
// #include "hostInterface.h"			// mainController contains a hostInterface
// #include "gcode.h"					// mainController contains a GCodeParser
// #include "motion.h"					// mainController contains a motionController


class mainController {
  private:
    // ### Properties ###
    // StateMachines
    mainState theMainState       = mainState::ready;
    homingState theHomingState   = homingState::lost;
    probingState theProbingState = probingState::lost;

    // IO Objects
    char commandLine[512];        // string to store message ready at host computer receiver interface
    char responseMsg[512];        // string to build message to report back to host computer
                                                                                             //		SDcard theStorage																		// SDcard storage for config files etc.
    // gCode theParser;                                                                         // contains a GCode parser to handle all gCode processing
    // gCodeParserResult theParseResult;                                                        // object to pass results from gCode parser on to machine motion control
    // Motion theMotion = Motion(theEventBuffer);                                               // contains a motion (controller) to handle all machine motion logic and calculations
    // ButtonSwitch switchesAndButtons[(uint8_t)limitSwitchButton::nmbrSwitchesButtons];        // array of objects handling limitswitches and general purpose buttons

    // Methods
    void handleEvents();          // handles the events such as switch close/open and end-of-motion
    void handleMessages();        // handles messages from hostInterface

  public:
    mainController();              // constructor
    void run();                    // read all inputs, process, generate outputs and statechange in an endless loop
};
