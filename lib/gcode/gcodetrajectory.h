// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "axis.h"

struct Trajectory {
    double length;        // length of the motion [mm] - either as linear or as helical

    // for linear
    double startPosition[nmbrAxis];        // [mm], per axis
    double delta[nmbrAxis];                // delta = nextPosition - currentPosition [mm], per axis

    // for helical
    axis arcAxis[2];            // which Axis forms the plane for the ARC
    double arcCenter[2];        // [mm]
    double radius;              // [mm]
    double startAngle;          // [rad]
    double deltaAngle;          // [rad]
} trajectory;                   // struct holding all GCODE trajectory parameters, to be passed to Motion
