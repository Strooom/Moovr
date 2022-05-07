// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "axis.h"
#include "gcodemodalgroup.h"

struct gCodeState
    {
    public:
        void initialize();

        double WCSorigin[nmbrCoordinateSets][nmbrAxis];		// Array to hold all WorldCoordinate settings
        uint8_t modalGroupsState[nmbrModalGroups];			// Array to hold all model groups settings
        double letterValueState[nmbrLetters];				// Array to hold value for each used gCode letter
        double currentPosition[nmbrAxis];					// current position [mm or degrees], before the motion
        double nextPosition[nmbrAxis];						// future position [mm or degrees], after the motion
    };
