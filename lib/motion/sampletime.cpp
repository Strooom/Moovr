#include "sampletime.h"

sampleTime::sampleTime(const float theMinStepPulseWidth) : minStepPulseWidth(theMinStepPulseWidth) {
}

void sampleTime::next() {
    sampleIndex++;
    timeInMotion = (static_cast<float>(sampleIndex) * minStepPulseWidth) + sampleZeroOffset;
}

void sampleTime::initialize() {
    sampleIndex      = 0;
    sampleZeroOffset = 0.0F;
    timeInMotion     = 0.0F;
}

bool sampleTime::isBeyondEndOfMotion() const {
    return (timeInMotion > motionDuration);
}

bool sampleTime::isBeyondStop() const {
    return (timeInMotion > motionTStop);
}

void sampleTime::initializeNextMotion() {
    sampleIndex      = 0;
    sampleZeroOffset = timeInMotion - motionDuration;
    timeInMotion     = sampleZeroOffset;
}

void sampleTime::setMotionTStop(float someStopTime) {
    motionTStop = someStopTime;
}
void sampleTime::setMotionDuration(float someMotionDuration) {
    motionDuration = someMotionDuration;
}
