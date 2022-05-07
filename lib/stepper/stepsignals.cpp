#include "stepsignals.h"
#include "machineproperties.h"

bool stepSignals::isModified() {
    return (dirSetup != lastOutput);
}

void stepSignals::next() {
    dirSetup = stepRise;           //
    stepRise = stepFall;           //
    ticksSinceLastOutput++;        //
}
void stepSignals::stepBackward(uint32_t axis) {
    // TODO : should we boundsCheck axis ?
    uint32_t mask = 0x00000001 << (2 * axis);        // initialize bitmask needed for manipulating Step and Dir bits
    dirSetup      = dirSetup & ~mask;                // mask step signal to zero
    stepRise      = stepRise | mask;                 // set step pulse high
    stepFall      = stepFall & ~mask;                // return step pulse low
    mask          = mask << 1;                       // adjust mask for direction bits
    dirSetup      = dirSetup | mask;                 // set direction high : 0 = stepping forward, 1 = stepping backward
    stepRise      = stepRise | mask;                 // set direction high
    stepFall      = stepFall | mask;                 // set direction high
}

void stepSignals::stepForward(uint32_t axis) {
    // TODO : should we boundsCheck axis ?
    uint32_t mask = 0x00000001 << (2 * axis);        // initialize bitmask needed for manipulating Step and Dir bits
    dirSetup      = dirSetup & ~mask;                // mask step signal to zero
    stepRise      = stepRise | mask;                 // set step pulse high
    stepFall      = stepFall & ~mask;                // return step pulse low
    mask          = mask << 1;                       // adjust mask for direction bits
    dirSetup      = dirSetup & ~mask;                // set direction low : 0 = stepping forward, 1 = stepping backward
    stepRise      = stepRise & ~mask;                // set direction low
    stepFall      = stepFall & ~mask;                // set direction low
}

step stepSignals::output() {
    lastOutput           = dirSetup;
    uint32_t timeBefore  = ticksSinceLastOutput;
    ticksSinceLastOutput = 0;
    return step{timeBefore, dirSetup};
}

step stepSignals::outputDefault() {
    lastOutput           = dirSetup;
    ticksSinceLastOutput = 0;
    return step{maxTicksSinceLastOutput, dirSetup};
}

bool stepSignals::isTimedOut() {
    return (ticksSinceLastOutput >= maxTicksSinceLastOutput);
}
