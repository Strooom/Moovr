// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#ifndef mainController_h
#define mainController_h

#ifndef WIN32
#include <Arduino.h>
#else
#include <inttypes.h>
#endif

#include "button.h"					// mainController contains an array of button objects
#include "event.h"					// mainController contains an eventBuffer
#include "hostInterface.h"			// mainController contains a hostInterface
#include "gcode.h"					// mainController contains a GCodeParser
#include "motion.h"					// mainController contains a motionController

#ifdef WIN32
const char ESC = '?';				/// ASCII Escape, all non GCode commands need ESC as first character on the line
#else
const char ESC = 0x1B;				/// ASCII Escape, all non GCode commands need ESC as first character on the line
#endif
const char CR = 0x0D;				/// ASCII Carriage Return, marks the end of the line
const char LF = 0x0A;				/// ASCII LineFeed, marks the end of the line

enum class Command : uint8_t
    {
    doHome = 'H',					///< initiates a homing cycle
    doProbe = 'P',					///< initiates a probing
    jog = 'J',						///< puts the machine into jogging mode
    uploadFile = 'U',
    executeFile = 'E',
    override = 'O',
    getStatus = '?',				///< request the current machine status
    cancel = 24,					///< cancel command
    gCode = 'G'						///< this is a placeholder for anything which is NOT a command, and is in fact treated like it's cGode
    };

enum class MainState : uint8_t
    {
    Ready,		///< the machine is ready to accept Commands or gCode
    Homing,		///< the machine is busy doing a homing cycle - further sub-state is controlled in HomingState
    Jogging,	///< the machine is in Jogging mode, it can receive a subset of gCode, some gCode context cannot be changed
    Probing,
    Running,	///< the machine is executing a gCode  (either from a file stored on SD card or from manual commands)
    Pausing,
    Loading,	///< a gCode fle is being uploaded from the Host to the SD-card of the controller
    Error		///< some unrecoverable error occured
    };

enum class HomingState : uint8_t
    {
    Lost,		// machine's home position is unknown : inject GCODE to search for closing the limitswitch

    ClosingZ,	// machine is busy searching for Z limitswitch to close, wait for close event
    ClosedZ,	// Z switch is closed -> stopping the motion by changing optimization strategy, wait for stop event
    OpeningZ,	// Z axis is stopped, inject GCODE to reopen limit switch, wait for open event
    OpenedZ,	// Z switch is opened -> stop the motion, wait for stop event
    // Z axis is stopped -> Zero is found, proceed with next axis..
    CLOSING_Y,
    CLOSED_Y,
    OPENING_Y,
    OPENED_Y,
    CLOSING_X,
    CLOSED_X,
    OPENING_X,
    OPENED_X,

    FOUND		//< machine has found it's X position -> fully homed.
    };

enum class ProbingState : uint8_t
    {
    Lost,
    Search,
    Found
    };

enum class limitSwitchButton : uint8_t
    {
    xMin, xMax, yMin, yMax, zMin, zMax, stop, hold, probe, nmbrSwitchesButtons
    };

class MainController
    {
    private:
        // ### Properties ###
        // StateMachines
        MainState mainState = MainState::Ready;													// instance to keep track of the main state of the FSM
        HomingState homingState = HomingState::Lost;											// instance keeping track of the homing sequence
        ProbingState probingState = ProbingState::Lost;											// instance keeping track of the probing sequence

        // IO Objects
        char commandLine[512];																	// string to store message ready at host computer receiver interface
        char responseMsg[512];																	// string to build message to report back to host computer
        EventBuffer theEventBuffer;

        // SubComponents objects
        HostInterfaceUart theHostInterface;														// MainController contains a host interface
//		SDcard theStorage																		// SDcard storage for config files etc.
        gCode theParser;																		// contains a GCode parser to handle all gCode processing
        gCodeParserResult theParseResult;														// object to pass results from gCode parser on to machine motion control
        Motion theMotion = Motion(theEventBuffer);												// contains a motion (controller) to handle all machine motion logic and calculations
        ButtonSwitch switchesAndButtons[(uint8_t)limitSwitchButton::nmbrSwitchesButtons];		// array of objects handling limitswitches and general purpose buttons

        // Methods
        void handleEvents();					// handles the events such as switch close/open and end-of-motion
        void handleMessages();					// handles messages from hostInterface

    public:
        MainController();						// constructor
        void run();								// read all inputs, process, generate outputs and statechange in an endless loop
        void serialInterrupt();					// handles the serial interrupts : byte received and byte transmitted
        void pit1Interrupt();					// handles the PIT0 timer interrupt : timer expired
        void pit2Interrupt();					// handles the PIT1 timer interrupt : timer expired
    };

#endif