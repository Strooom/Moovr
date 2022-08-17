// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

//#include <math.h>
#include "gcodestate.h"

void gCodeState::initialize() {
    // Initialize all Model groups
    modalGroupsState[static_cast<uint32_t>(modalGroup::Motion)]         = static_cast<uint32_t>(modalGroupMotion::G0);
    modalGroupsState[static_cast<uint32_t>(modalGroup::FeedRate)]       = static_cast<uint32_t>(modalGroupFeedRate::G94);
    modalGroupsState[static_cast<uint32_t>(modalGroup::Spindle)]        = static_cast<uint32_t>(modalGroupSpindle::M5);
    modalGroupsState[static_cast<uint32_t>(modalGroup::Unit)]           = static_cast<uint32_t>(modalGroupUnit::G21);
    modalGroupsState[static_cast<uint32_t>(modalGroup::Unit)]           = static_cast<uint32_t>(modalGroupUnit::G21);
    modalGroupsState[static_cast<uint32_t>(modalGroup::Plane)]          = static_cast<uint32_t>(modalGroupPlane::G17);
    modalGroupsState[static_cast<uint32_t>(modalGroup::Distance)]       = static_cast<uint32_t>(modalGroupDistance::G90);
    modalGroupsState[static_cast<uint32_t>(modalGroup::CoordinateSet)]  = static_cast<uint32_t>(modalGroupCoordinateSet::G54);
    modalGroupsState[static_cast<uint32_t>(modalGroup::CoolantFlood)]   = static_cast<uint32_t>(modalGroupCoolantFlood::M9);
    modalGroupsState[static_cast<uint32_t>(modalGroup::CoolantMist)]    = static_cast<uint32_t>(modalGroupCoolantMist::M9);
    modalGroupsState[static_cast<uint32_t>(modalGroup::ManualOverride)] = static_cast<uint32_t>(modalGroupOverrides::M48);

    // initialize all letterValues
    for (uint32_t theGCodeLetter = static_cast<uint32_t>(gCodeLetter::X); theGCodeLetter < nmbrLetters; theGCodeLetter++) {
        letterValueState[theGCodeLetter] = 0.0;        // Array to hold value for each used gCode letter
    }

    for (uint32_t theAxis = 0; theAxis < nmbrAxis; ++theAxis)        // iterate over all axis
    {
        currentPosition[theAxis] = 0.0;
    }

    // start from compile-time defaults
    for (uint8_t theModalGroupCoordinateSet = 0; theModalGroupCoordinateSet < nmbrCoordinateSets; theModalGroupCoordinateSet++) {
        for (uint8_t theAxis = static_cast<uint32_t>(axis::X); theAxis < nmbrAxis; theAxis++) {
            WCSorigin[theModalGroupCoordinateSet][theAxis] = 0;
        }
    }
}
