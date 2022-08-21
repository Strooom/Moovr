#include <limits>
#include "sampletime.h"

sampleTime::sampleTime() {
    initialize();
}

// sampleTime::sampleTime(const float theMinStepPulseWidth) : minStepPulseWidth(theMinStepPulseWidth) {
// }

void sampleTime::next() {
    sampleIndex++;
    timeInMotion = (static_cast<float>(sampleIndex) * minStepPulseWidth) + sampleZeroOffset;
}

void sampleTime::initialize() {
    sampleIndex      = 0;
    sampleZeroOffset = 0.0f;
    timeInMotion     = 0.0f;
}

bool sampleTime::isBeyond(float someTime) const {
    return (timeInMotion > someTime);
}

void sampleTime::initializeNextMotion() {
    sampleIndex    = 0;
    timeInMotion   = sampleZeroOffset;
}

void sampleTime::setminStepPulseWidth(const float theMinStepPulseWidth) {
    minStepPulseWidth = theMinStepPulseWidth;
}