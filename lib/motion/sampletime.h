// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class sampleTime {
  public:
    sampleTime(const float theMinStepPulseWidth);        // constructor
    void next();                                         // calculate the position of the next sampling point
    void initialize();                                   //
    bool isBeyondEndOfMotion() const;                    //
    bool isBeyondStop() const;                           //
    void initializeNextMotion();                         //
    void setMotionTStop(float someStopTime);
    void setMotionDuration(float someMotionDuration);

#ifndef unitTesting
  private:
#endif
    friend class motionCtrl;
    float timeInMotion{0.0F};             // time [s] between beginning of the motion and the current sampling point
    float sampleZeroOffset{0.0F};         // time [s] between start of motion and first (index == 0) sample
    uint32_t sampleIndex{0};              // zero-based index of sampling points inside the current motion
    float motionTStop{0.0F};              // time after which the motion comes to a stop [s]
    float motionDuration{0.0F};           // duration [s] of the complete motion
    const float minStepPulseWidth;        // duration [s] of a single discrete timeTick, equals the time [s] between the sampling points
};
