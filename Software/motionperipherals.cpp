#include "motionitemperipherals.h"

void motionPeripherals::print() const
    {
#ifndef  WIN32
    Serial.println("Peripheral  : ");
#endif
    }

void motionPeripherals::set(const gCodeParserResult &theParseResult)
    {
    spindledirection = theParseResult.motion.peripherals.spindledirection;
    spindleSpeed = static_cast<float>(theParseResult.motion.peripherals.spindleSpeed);
    coolantFlood = theParseResult.motion.peripherals.coolantFlood;
    coolantMist = theParseResult.motion.peripherals.coolantMist;
    }
