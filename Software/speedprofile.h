#pragma once

// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "general.h"        // required for definition of MotionSpeedProfile

class SpeedProfileAccDec {
  public:
    SpeedProfileAccDec();
    void setMax(float aMax, float dMax, float jMax);
    void setvStart(float vStart);
    void setvEnd(float vEnd);

    void calculate(MotionSpeedProfileOrder theProfile);        // input vStart, vEnd, length,....   then calculate everything else : timings and distances travelled
    float calcSpeed() const;                                   // calculate the exit speed to a T-profile section, given vStart, (wanted but not reached) vEnd, length, aMax, dMax

    float a(float t) const;        // calculated acceleration at time t
    float v(float t) const;        // calculated speed at time t
    float s(float t) const;        // calculated distance travelled at time t

    uint32_t toString(char *output) const;

    static constexpr float oneSixth = 1.0F / 6.0F;        // constant to avoid having to divide by 6, as division is slower than multiplication

    // TODO : check about private here..

    // Inputs
    float vStart;        // speed at entry of the section [mm/s]
    float vEnd;          // speed at exit of the section [mm/s]
    float aMax;          // maximum acceleration along the path [mm/ s^2]
    float dMax;          // maximum deceleration along the path [mm/ s^2]
    float jMax;          // maximum jerk = machine constant [mm / s^3]

    // Outputs - Speed
    float dV1;        // change in speed over phase 1

    // Outputs - Distance
    float s1;            // length of phase 1
    float s2;            // length of phase 2
    float s3;            // length of phase 3
    float length;        // total length of the motionPart [mm]

    // Outputs - Timing
    float d1d3;            // duration of phase 1 = duration of phase 3. Zero for T-profile [s]
    float d2;              // duration of phase 2. Is the only phase for T-profile [s]
    float duration;        // duration of phase 2. Is the only phase for T-profile [s]
};

class SpeedProfileCruise {
  public:
    SpeedProfileCruise();
    void set(float vMid);
    void print() const;

    float a(float t) const;        // calculated acceleration at time t
    float v(float t) const;        // calculated speed at time t
    float s(float t) const;        // calculated distance travelled at time t

    uint32_t toString(char *output) const;

  private:
  public:        // for Unit testing, so all tests can read all members
    // Inputs
    float vMid;        // speed during the motion mid part [mm/s]

    // Outputs - Distance
    float length;        // total length of the motionPart [mm]

    // Outputs - Timing
    float duration;        // duration of motionPart [s]
};