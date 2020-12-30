// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "gcodeparseresult.h"        // required as gCodeParseResult is passed as a parameter to some memberfunctions
#include "general.h"                 // required for definition of MotionSpeedProfile

class motionPeripherals {
  public:
    uint32_t toString(char *output) const;
    void set(const gCodeParserResult &theParseResult);

    rotationDirection spindledirection{rotationDirection::Clockwise};
    float spindleSpeed{};
    bool coolantFlood{};
    bool coolantMist{};
};
