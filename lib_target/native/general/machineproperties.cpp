// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "machineproperties.h"

void machineProperties::load() {
    // TODO : load the properties from a Json file on SD card..
}

void machineProperties::save() {
    // TODO : save the properties to an SD card or other stream
}

void machineProperties::setForTest(uint32_t aSet) {
    switch (aSet) {
        case 0U:
            motors.jMax = 1000.0f;
            limits.maxLimitswitchTravel = 2.0f;
            for (uint32_t anAxis = 0; anAxis < nmbrAxis; anAxis++) {
                motors.aMax[anAxis]       = 200.0f;
                motors.dMax[anAxis]       = -200.0f;
                motors.vMax[anAxis]       = 200.0f;
                motors.sMax[anAxis]       = 1000.0f;
                motors.stepsPerMm[anAxis] = 50.0f;
                limits.hasLimitsMax[anAxis] = true;
                limits.hasLimitsMin[anAxis] = true;
            }
            minLengthSProfile = 100.0f;
            vMaxHoming = motors.jMax * motors.jMax * motors.jMax * oneSixth;
            break;

        case 1U:
        default:
            for (uint32_t anAxis = 0; anAxis < nmbrAxis; anAxis++) {
                motors.aMax[anAxis] = 1.0f;
                motors.dMax[anAxis] = -1.0f;
                motors.vMax[anAxis] = 1.0f;
                motors.stepsPerMm[anAxis] = 4.0f;
            }
            minLengthSProfile = 1000.0f;
            break;
    }
}
