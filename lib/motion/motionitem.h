// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include <stdint.h>
#include "overrides.h"
#include "motionstrategy.h"
#include "motioncalculatedirection.h"

#include "machineproperties.h"

#include "motiontype.h"
#include "motiontrajectory.h"
#include "motionspeedprofile.h"
#include "motionperipherals.h"

class motion {
  public:
    void set(const simplifiedMotion &theMotion, overrides theOverrides, motionStrategy theStrategy);

    void limit();                                                                         // calculate vMax, aMax and dMax from trajectory and machine limits
    void optimize(overrides theOverrides, motionStrategy theStrategy, float tNow);        // optimizes this motion
    void optimize(overrides theOverrides, motionStrategy theStrategy);                    // optimizes this motion

    // void optimizeOld();                      // optimizes this motion

    float vTri(motionStrategy strategy) const;
    static float vTri(float vStart, float vEnd, float aMax, float dMax, float length, motionStrategy strategy);
    float vOut(motionStrategy strategy, motionCalculateDirection direction) const;
    static float vOut(float vIn, float aMax, float dMax, float length, motionStrategy strategy, motionCalculateDirection direction);

    float s(float t) const;        // calculates s : distance travelled along the trajectory on time t
    float v(float t) const;        // calculates v : speed along the trajectory on time t
    float a(float t) const;        // calculates a : acceleration (or deceleration, which is just negative) along the trajectory on time t

    bool isMoving(uint8_t axis) const;

    uint32_t toString(char *output) const;

#ifndef unitTesting
  private:
#endif
    friend class motionBuffer;
    friend class motionCtrl;

    motionType type{motionType::none};        //
    motionTrajectory trajectory;              // contains all properties for defining the trajectory in space
    motionSpeedProfile speedProfile;          // contains all properties for defining the speed over the trajectory in time
    motionPeripherals peripherals;            // contains all settings for peripherals over this part of the trajectory
};