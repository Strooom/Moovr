// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>                    //
#include "simplifiedmotion.h"          //
#include "speedprofileaccdec.h"        //
#include "speedprofilecruise.h"        //
#include "speedprofilepassed.h"        //

class motionSpeedProfile {
  public:
    void setSpeed(const simplifiedMotion &theMotion);           // length and vFeed are inputs, and speedProfile is calculated
    void setDuration(const simplifiedMotion &theMotion);        // duration is input, speed is zero

    uint32_t toString(char *output) const;

#ifndef unitTesting
  private:
#endif
    friend class motion;
    friend class motionBuffer;
    friend class motionCtrl;
    
    speedProfilePassed done;         // part already executed, in case the current motion needs to be recalculated
    speedProfileAccDec left;         // left part, usually the acceleration
    speedProfileCruise mid;          // mid part, the cruise
    speedProfileAccDec right;        // right part, usually the deceleration

    float vFeed{};        // this is the (max) speed as 'wanted' by the GCode
    float vMax{};         // this is the max speed as limited by machine constraints and overrides
    float aMax{};         // this is the max acceleration as limited by machine constraints [mm/s^2], >=0
    float dMax{};         // this is the max decelerationspeed as limited by machine constraints [mm/s^2], <=0
    float jMax{};         // jMax is always as set in machineProperties and cannot be changed but is copied here to make things simpler

    float duration{};        // [s] total duration of the motion
    float tStop{};           // time at which the motion will come to a stop - for feedhold.. if no stop needed, then this holds infinity
};