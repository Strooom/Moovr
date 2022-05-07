// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class plane : uint32_t {
    XY,
    YZ,
    ZX,
    nmbrPlanes
};

constexpr uint32_t nmbrPlanes = static_cast<uint32_t>(plane::nmbrPlanes);
const char* toString(plane aPlane);