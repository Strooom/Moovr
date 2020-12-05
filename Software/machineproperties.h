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
#include "general.h"        // access to axis, nmbrAxis, ...

class MachineProperties {
  public:
    void load();
    void save();

    struct Motors {
        float jMax{10.0F};                                      // [mm/s^3]
        float aMax[nmbrAxis]{10.0F, 10.0F, 10.0F};              // [mm/s^2]
        float dMax[nmbrAxis]{-10.0F, -10.0F, -10.0F};           // [mm/s^2]
        float vMax[nmbrAxis]{200.0F, 200.0F, 200.0F};           // [mm/s]
        float sMax[nmbrAxis]{600.0F, 800.0F, 130.0F};           // [mm]
        float stepsPerMm[nmbrAxis]{50.0F, 50.0F, 50.0F};        // [1/mm]
    } motors;

    struct Limits {
        bool hasLimitsMin[nmbrAxis]{true, true, true};        // limit switches towards the negative direction of the Axis
        bool hasLimitsMax[nmbrAxis]{true, true, true};        // limit switches towards the positive direction of the Axis
        float maxLimitswitchTravel{2.0F};                     // [mm]
    } limits;

    struct Spindle {
        bool hasVariableSpindle{false};
        bool hasSpindledirection{false};
        float minSpindleRPM{10000.0F};
        float maxSpindleRPM{30000.0F};
    } spindle;

    struct Coolant {
        bool hasCoolantFlood{true};
        bool hasCoolantMist{true};
    } coolant;

    double minLengthSProfile{100.0F};        // [mm] all motions with a length smaller will be 2nd order T-profile - larger will be 3rd order S-profile
};

struct Overrides {
    float feedOverride{1.0F};           // feed override factor [0.1 .. 10]
    float spindleOverride{1.0F};        // spindle speed overrride factor [0.1 .. 10]
};
