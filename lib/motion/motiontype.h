// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class motionType : uint32_t {
    traverse,              // 3D Linear move at maximum speed = G0
    feedLinear,            // 3D Linear move at feedrate speed = G1
    feedHelicalCW,         // Helical move at feedrate speed = G2
    feedHelicalCCW,        // Helical move at feedrate speed = G3
    pauseAndResume,        // Pause for a defined time, then resume = G4
    pause,                 // Pause for undefined time, resume on resume commmand = M0 or M1
    none                   // When parsing a gCode block results in no motion, the motionType = None
};
const char* toString(motionType aMotionType);
