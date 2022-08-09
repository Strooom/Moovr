// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "speedprofilepassed.h"
#include <stdio.h>        // needed for sprintf()

speedProfilePassed::speedProfilePassed() : length{0.0F}, duration{0.0F} {
}

uint32_t speedProfilePassed::toString(char* output) const {
    uint32_t outputLenght{0};

    outputLenght += sprintf(output + outputLenght, "length   = %f mm\n", length);
    outputLenght += sprintf(output + outputLenght, "duration = %f s\n", duration);

    return outputLenght;
}