#include "motionspeedprofile.h"
#include <stdio.h>

void motionSpeedProfile::setSpeed(const simplifiedMotion &theMotion) {
    vFeed = static_cast<float>(theMotion.speedProfile.vFeed);
    left.setMax(aMax, dMax, jMax);
    left.setvStart(0.0F);
    right.setMax(aMax, dMax, jMax);
    right.setvEnd(0.0F);
}

void motionSpeedProfile::setDuration(const simplifiedMotion &theMotion) {
    duration = static_cast<float>(theMotion.speedProfile.duration);
    // TODO : set left, mid and right part of the profile
}

uint32_t motionSpeedProfile::toString(char *output) const {
    uint32_t outputLenght{0};
    //    outputLenght += sprintf(output + outputLenght, "SpeedProfile  :\n");
    outputLenght += sprintf(output + outputLenght, "feedrate =  %f  mm/s\n", vFeed);
    outputLenght += sprintf(output + outputLenght, "duration =  %f  s\n", duration);

    outputLenght += sprintf(output + outputLenght, "left part :\n");
    outputLenght += left.toString(output + outputLenght);
    outputLenght += sprintf(output + outputLenght, "mid part :\n");
    outputLenght += mid.toString(output + outputLenght);
    outputLenght += sprintf(output + outputLenght, "right part :\n");
    outputLenght += right.toString(output + outputLenght);

    return outputLenght;
}
