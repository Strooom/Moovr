// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class homingState : uint32_t {
    lost,                         // homing state for this axis unknown
    stopping,                     // stopping any ongoing motion before starting homing motions
    closing,                      // machine is running towards closing a limitswitch
    closedWaitForStop,            // switch is closed -> stopping the motion by changing optimization strategy, waiting for stop event
    opening,                      // axis is stopped, start motion to reopen limit switch, wait for open event
    openedWaitForStop,            // switch is opened -> stop the motion, wait for stop event
    offsettingWaitForStop,        // axis is stopped -> move now to predefined offset from limitswitch, wait for stop
    found                         // machine has arrived at offset from limit switch, home is found
};

const char* toString(homingState aHomingState);