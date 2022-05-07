// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class homingState : uint32_t {
    lost,            // machine's home position is unknown : inject GCODE to search for closing the limitswitch
    closingZ,        // machine is busy searching for Z limitswitch to close, wait for close event
    closedZ,         // Z switch is closed -> stopping the motion by changing optimization strategy, wait for stop event
    openingZ,        // Z axis is stopped, inject GCODE to reopen limit switch, wait for open event
    openedZ,         // Z switch is opened -> stop the motion, wait for stop event
    closingY,        // Z axis is stopped -> Zero is found, proceed with next axis..
    closedY,         //
    openingY,        //
    openedY,         //
    closingX,        //
    closedX,         //
    openingX,        //
    openedX,         //
    found            //  machine has found it's X position -> fully homed.
};


const char* toString(homingState aHomingState);