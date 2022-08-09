#include "motionperipherals.h"
#include <stdio.h>

uint32_t motionPeripherals::toString(char *output) const {
    uint32_t outputLenght{0};
    outputLenght += sprintf(output + outputLenght, "Peripheral  : \n");
    return outputLenght;
}

void motionPeripherals::set(const simplifiedMotion &theMotion) {
    spindledirection = theMotion.peripherals.spindledirection;
    spindleSpeed     = static_cast<float>(theMotion.peripherals.spindleSpeed);
    coolantFlood     = theMotion.peripherals.coolantFlood;
    coolantMist      = theMotion.peripherals.coolantMist;
}
