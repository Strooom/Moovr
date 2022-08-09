// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class axis : uint32_t {
    X,
    Y,
    nmbrAxis
};

constexpr uint32_t nmbrAxis = static_cast<uint32_t>(axis::nmbrAxis);
const char* toString(axis anAxis);