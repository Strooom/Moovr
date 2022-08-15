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
        default:
            for (uint32_t anAxis = 0; anAxis < nmbrAxis; anAxis++) {
                motors.aMax[anAxis] = 1.0f;
                motors.dMax[anAxis] = -1.0f;
                motors.vMax[anAxis] = 1.0f;
            }
            break;
    }
}