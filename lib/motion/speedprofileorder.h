// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include <stdint.h>

enum class speedProfileOrder : uint32_t {
    firstOrder,
    secondOrder
};        // Shape of the speedprofile : first or second order

const char* toString(speedProfileOrder aMotionSpeedProfileOrder);