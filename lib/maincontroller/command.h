#pragma once
#include "stdint.h"

#ifdef WIN32
const char ESC = '?';        // ASCII Escape, all non GCode commands need ESC as first character on the line
#else
const char ESC = 0x1B;        // ASCII Escape, all non GCode commands need ESC as first character on the line
#endif
const char CR = 0x0D;        // ASCII Carriage Return, marks the end of the line
const char LF = 0x0A;        // ASCII LineFeed, marks the end of the line

enum class command : uint32_t {
    doHome      = 'H',        // initiates a homing cycle
    doProbe     = 'P',        // initiates a probing
    setOverride = 'O',
    getStatus   = '?',        // request the current machine status
    cancel      = 24,         // cancel command
    gCode       = 'G'         // this is a placeholder for anything which is NOT a command, and is in fact treated like it's cGode
};
