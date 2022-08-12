// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

// when we optimize the motion, we can aim either for going as fast as possible (normal running mode) or as slow as possible (stopping or stopped)

enum class motionStrategy : uint32_t {
    minimizeSpeed,
    maximizeSpeed
};

const char* toString(motionStrategy aStrategy);