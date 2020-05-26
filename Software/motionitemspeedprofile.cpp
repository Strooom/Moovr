#include "MotionItemSpeedProfile.h"

MotionItemSpeedProfile::MotionItemSpeedProfile()
    {
    }

void MotionItemSpeedProfile::print() const
    {
#ifndef  WIN32
    Serial.println("SpeedProfile  : ");

    Serial.print("  feedrate = ");
    Serial.print(vFeed);
    Serial.print(" mm/s\n");

    Serial.print("  duration = ");
    Serial.print(duration);
    Serial.print(" s\n");

    //left.print();
    //mid.print();
    //right.print();
#endif
    }

void MotionItemSpeedProfile::setSpeed(const gCodeParserResult &theParseResult)
    {
    vFeed = static_cast<float>(theParseResult.motion.speedProfile.vFeed);
    left.setMax(aMax, dMax, jMax);
    left.setvStart(0.0F);
    right.setMax(aMax, dMax, jMax);
    right.setvEnd(0.0F);
    }

void MotionItemSpeedProfile::setDuration(const gCodeParserResult &theParseResult)
    {
    duration = static_cast<float>(theParseResult.motion.speedProfile.duration);
    // TODO : set left, mid and right part of the profile
    }

