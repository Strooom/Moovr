#include "motionspeedprofile.h"

void motionSpeedProfile::print() const {
#ifndef WIN32
    Serial.println("SpeedProfile  : ");

    Serial.print("  feedrate = ");
    Serial.print(vFeed);
    Serial.print(" mm/s\n");

    Serial.print("  duration = ");
    Serial.print(duration);
    Serial.print(" s\n");

    ////left.print();
    //mid.print();
    //right.print();
#endif
}

void motionSpeedProfile::setSpeed(const gCodeParserResult &theParseResult) {
    vFeed = static_cast<float>(theParseResult.motion.speedProfile.vFeed);
    left.setMax(aMax, dMax, jMax);
    left.setvStart(0.0F);
    right.setMax(aMax, dMax, jMax);
    right.setvEnd(0.0F);
}

void motionSpeedProfile::setDuration(const gCodeParserResult &theParseResult) {
    duration = static_cast<float>(theParseResult.motion.speedProfile.duration);
    // TODO : set left, mid and right part of the profile
}
