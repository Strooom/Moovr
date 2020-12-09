#pragma once

#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "gcodeparseresult.h"
#include "machineproperties.h"
#include "motionperipherals.h"
#include "motionspeedprofile.h"
#include "motiontrajectory.h"

class motion {
  public:
    void set(const gCodeParserResult &theParseResult, const machineProperties &theMachineProperties, const MotionStrategy &theStrategy, const overrides &theOverrides);        // sets the properties of an Item based upon inputs from gcodeParseResult and machineProperties

    void limit(const machineProperties &theMachineProperties);                       // calculate vMax, aMax and dMax from trajectory and machine limits
    void optimize(MotionStrategy theStrategy, const overrides &theOverrides);        // optimizes this motion

    void adjustRemaining();
    float calcTriangular(const MotionStrategy theStrategy);
    float calcOtherV(const MotionStrategy theStrategy, bool forward);

    float s(float t) const;        // calculates s : distance travelled along the trajectory on time t
    float v(float t) const;        // calculates v : speed along the trajectory on time t
    float a(float t) const;        // calculates a : acceleration (or deceleration, which is just negative) along the trajectory on time t

    bool isMoving(uint8_t axis) const;

    uint32_t toString(char *output) const;                                                                                                                 // prints a diagram to Serial for evaluating and debugging
    uint32_t motion::plot(char *output, char type, float xMin, float xMax, uint32_t nmbrXSteps, float yMin, float yMax, uint32_t nmbrYSteps) const;        // plots a chart to Serial for evaluating and debugging. Type is 'a', 'v' or 's'

    //float vMid2(overrides &theOverrides);				// calculates maximum vMid from vFeed, vLimit and feed overrides
    //void fromParts(const SpeedProfileAccDec &left, const SpeedProfileAccDec &right, float d4, float len4);	// load the motion properties, from two MotionParts

    //private:
    MotionType theType;
    motionTrajectory theTrajectory;            // contains all properties for defining the trajectory in space
    motionSpeedProfile theSpeedProfile;        // contains all properties for defining the speed over the trajectory in time
    motionPeripherals thePeripherals;          // contains all settings for peripherals over this part of the trajectory
};