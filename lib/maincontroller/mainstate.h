// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
enum class mainState : uint8_t {
    ready,          // the machine is in Jogging mode, it can receive a subset of gCode, some gCode context cannot be changed
    homing,         // the machine is busy doing a homing cycle - further sub-state is controlled in homingStates
    probing,        // the machine is busy doing a probing cycle - further sub-state is controlled in probingStates
    running,        // the machine is executing a gCode program
    error           // some unrecoverable error occured
};

const char *toString(mainState aState);