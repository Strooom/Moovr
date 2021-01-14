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

enum class motionStrategy : uint8_t {
    minimizeSpeed,
    maximizeSpeed
};

enum class motionCalculateDirection : uint8_t {
    forward,
    reverse
};

class motion {
  public:
    void set(const gCodeParserResult &theParseResult, const machineProperties &theMachineProperties, const motionStrategy &theStrategy, const overrides &theOverrides);        // sets the properties of an Item based upon inputs from gcodeParseResult and machineProperties

    void limit(const machineProperties &theMachineProperties);                                                                               // calculate vMax, aMax and dMax from trajectory and machine limits
    void optimizeCurrent(const motionStrategy theStrategy, const overrides &theOverrides, float tNow);                                       // optimizes this motion
    void optimizeOld(const motionStrategy theStrategy, const overrides &theOverrides, const machineProperties &theMachineProperties);        // optimizes this motion
    void optimize(const motionStrategy theStrategy, const overrides &theOverrides);                                                          // optimizes this motion

    float vTri(motionStrategy strategy) const;
    float vTri(float vStart, float vEnd, float aMax, float dMax, float length, motionStrategy strategy) const;
    float vOut(motionStrategy strategy, motionCalculateDirection direction) const;
    float vOut(float vIn, float aMax, float dMax, float length, motionStrategy strategy, motionCalculateDirection direction) const;

    float s(float t) const;        // calculates s : distance travelled along the trajectory on time t
    float v(float t) const;        // calculates v : speed along the trajectory on time t
    float a(float t) const;        // calculates a : acceleration (or deceleration, which is just negative) along the trajectory on time t

    bool isMoving(uint8_t axis) const;

    uint32_t toString(char *output) const;        // prints a diagram to Serial for evaluating and debugging
    void export2csv(const char *outputFilename, uint32_t nmbrDataPoints);
    void setForTest(uint32_t type);

    motionType type{motionType::None};        //
    motionTrajectory trajectory;              // contains all properties for defining the trajectory in space
    motionSpeedProfile speedProfile;          // contains all properties for defining the speed over the trajectory in time
    motionPeripherals peripherals;            // contains all settings for peripherals over this part of the trajectory

#ifndef UnitTesting        //
  private:                 // commented out during unit testing
#endif                     //
};