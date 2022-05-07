#include "sampletime.h"
#include "machineproperties.h"

sampleTime::sampleTime(motionBuffer& aBuffer) : theMotionBuffer(aBuffer) {
}

void sampleTime::next() {
    sampleIndex++;
    timeInMotionFloat = (static_cast<float>(sampleIndex) * minStepPulseWidth) + sampleZeroOffset;
}

void sampleTime::reset() {
    sampleIndex       = 0;
    sampleZeroOffset  = 0.0F;
    timeInMotionFloat = 0.0F;
}

bool sampleTime::isBeyondEndOfMotion() const {
    return (timeInMotionFloat > theMotionBuffer.getHeadPtr()->speedProfile.duration);
}

bool sampleTime::isBeyondStop() const {
    return (timeInMotionFloat > theMotionBuffer.getHeadPtr()->speedProfile.tStop);
}

void sampleTime::nextMotion() {
    sampleIndex      = 0;
    sampleZeroOffset = timeInMotionFloat - theMotionBuffer.getHeadPtr()->speedProfile.duration;
    timeInMotionFloat = sampleZeroOffset;
}
