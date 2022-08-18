#include <limits>
#include "sampletime.h"

sampleTime::sampleTime() {
}

// sampleTime::sampleTime(const float theMinStepPulseWidth) : minStepPulseWidth(theMinStepPulseWidth) {
// }

void sampleTime::next() {
    sampleIndex++;
    timeInMotion = (static_cast<float>(sampleIndex) * minStepPulseWidth) + sampleZeroOffset;
}

void sampleTime::initialize() {
    sampleIndex      = 0;
    sampleZeroOffset = 0.0F;
    timeInMotion     = 0.0F;
    motionTStop      = std::numeric_limits<float>::infinity();
}

bool sampleTime::isBeyondEndOfMotion() const {
    return (timeInMotion > motionDuration);
}

bool sampleTime::isBeyondStop() const {
    return (timeInMotion > motionTStop);
}

void sampleTime::cleanupCurrentMotion() {
    sampleZeroOffset = timeInMotion - motionDuration;
}

void sampleTime::initializeNextMotion(float someMotionDuration, float someStopTime) {
    sampleIndex    = 0;
    motionDuration = someMotionDuration;
    motionTStop    = someStopTime;
    timeInMotion   = sampleZeroOffset;
}

void sampleTime::setMotionTStop(float someStopTime) {
    motionTStop = someStopTime;
}
void sampleTime::setMotionDuration(float someMotionDuration) {
    motionDuration = someMotionDuration;
}

void sampleTime::setminStepPulseWidth(const float theMinStepPulseWidth) {
    minStepPulseWidth = theMinStepPulseWidth;
}