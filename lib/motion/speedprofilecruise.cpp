// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "speedprofilecruise.h"
#include <stdio.h>        // needed for sprintf()

speedProfileCruise::speedProfileCruise() : vMid{0.0F}, length{0.0F}, duration{0.0F} {
}

void speedProfileCruise::set(float v) {
    vMid = v;
}

float speedProfileCruise::a(float t) const {
    return 0.0F;
}

float speedProfileCruise::v(float t) const {
    return vMid;
}

float speedProfileCruise::s(float t) const {
    return vMid * t;
}

uint32_t speedProfileCruise::toString(char* output) const {
    uint32_t outputLenght{0};

    outputLenght += sprintf(output + outputLenght, "vMid     = %f mm/s\n", vMid);
    outputLenght += sprintf(output + outputLenght, "length   = %f mm\n", length);
    outputLenght += sprintf(output + outputLenght, "duration = %f s\n", duration);

    return outputLenght;
}

