// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

//#include <math.h>
#include "gcodestate.h"

void gCodeState::initialize()
    {
    // Initialize all Model groups
    modalGroupsState[(uint8_t) modalGroup::Motion] = (uint8_t) modalGroupMotion::G0;
    modalGroupsState[(uint8_t) modalGroup::FeedRate] = (uint8_t) modalGroupFeedRate::G94;
    modalGroupsState[(uint8_t) modalGroup::Spindle] = (uint8_t) modalGroupSpindle::M5;
    modalGroupsState[(uint8_t) modalGroup::Unit] = (uint8_t) modalGroupUnit::G21;
    modalGroupsState[(uint8_t) modalGroup::Unit] = (uint8_t) modalGroupUnit::G21;
    modalGroupsState[(uint8_t) modalGroup::Plane] = (uint8_t) modalGroupPlane::G17;
    modalGroupsState[(uint8_t) modalGroup::Distance] = (uint8_t) modalGroupDistance::G90;
    modalGroupsState[(uint8_t) modalGroup::CoordinateSet] = (uint8_t) modalGroupCoordinateSet::G54;
    modalGroupsState[(uint8_t) modalGroup::CoolantFlood] = (uint8_t) modalGroupCoolantFlood::M9;
    modalGroupsState[(uint8_t) modalGroup::CoolantMist] = (uint8_t) modalGroupCoolantMist::M9;
    modalGroupsState[(uint8_t) modalGroup::ManualOverride] = (uint8_t) modalGroupOverrides::M48;

    // initialize all letterValues
    for (uint8_t theGCodeLetter = (uint8_t) gCodeLetter::X; theGCodeLetter < nmbrLetters; theGCodeLetter++)
        {
        letterValueState[theGCodeLetter] = 0.0;							// Array to hold value for each used gCode letter
        }

    for (uint8_t theAxis = 0; theAxis < nmbrAxis; ++theAxis)			// iterate over all axis
        {
        currentPosition[theAxis] = 0.0;
        }

    // start from compile-time defaults
    for (uint8_t theModalGroupCoordinateSet = 0; theModalGroupCoordinateSet < nmbrCoordinateSets; theModalGroupCoordinateSet++)
        {
        for (uint8_t theAxis = (uint8_t) axis::X; theAxis < nmbrAxis; theAxis++)
            {
            WCSorigin[theModalGroupCoordinateSet][theAxis] = 0;
            }
        }
    }
