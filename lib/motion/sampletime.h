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
    sampleTime();        // constructor
    // sampleTime(const float theMinStepPulseWidth);                       // constructor
    void next();                                                        // calculate the position of the next sampling point
    void initialize();                                                  //
    bool isBeyond(float someTime) const;           //
    float getRemainingTime();                                           //
    void initializeNextMotion();               //
    void setminStepPulseWidth(const float theMinStepPulseWidth);        //

#ifndef unitTesting
  private:
#endif
    friend class motionCtrl;
    float timeInMotion{0.0F};             // time [s] between beginning of the motion and the current sampling point
    float sampleZeroOffset{0.0F};         // time [s] between start of motion and first (index == 0) sample
    uint32_t sampleIndex{0};              // zero-based index of sampling points inside the current motion
    float minStepPulseWidth{1.0F};        // duration [s] of a single discrete timeTick, equals the time [s] between the sampling points
};
