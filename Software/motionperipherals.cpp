#include "motionperipherals.h"
#include <stdio.h>

uint32_t motionPeripherals::toString(char *output) const {
    uint32_t outputLenght{0};
    outputLenght += sprintf(output + outputLenght, "Peripheral  : \n");
    return outputLenght;
}

void motionPeripherals::set(const gCodeParserResult &theParseResult) {
    spindledirection = theParseResult.motion.peripherals.spindledirection;
    spindleSpeed     = static_cast<float>(theParseResult.motion.peripherals.spindleSpeed);
    coolantFlood     = theParseResult.motion.peripherals.coolantFlood;
    coolantMist      = theParseResult.motion.peripherals.coolantMist;
}
