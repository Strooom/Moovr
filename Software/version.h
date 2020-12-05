// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include "logging.h"

extern uLog theLog;

class version {
  public:
    explicit version(uint32_t a, uint32_t b, uint32_t c) : mainVersion(a), subVersion(b), minorVersion(c) {
        theLog.snprintf(loggingLevel::Info, "Moovr V%d.%d.%d", mainVersion, subVersion, minorVersion);
    }
    const uint32_t mainVersion;
    const uint32_t subVersion;
    const uint32_t minorVersion;
};

// -----------------------------------------------------------------------
// Release Notes
// -----------------------------------------------------------------------

// V0.0.1 - Nov 2020
// Separating HW specific code (HW timers, Serial, ..) from generic code (gCode parser, main control stateMachine)
// Reorganized everything in one Visual Studio Solution, which holds the application, the target unit tests and the windows unit tests
