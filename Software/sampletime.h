#pragma once

// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#ifndef WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif

#include "motionbuffer.h"

class sampleTime {
  public:
    sampleTime(motionBuffer &aBuffer);        // constructor
    void next();                              // calculate the position of the next sampling point
    void reset();                             //
    bool isBeyondEndOfMotion() const;         //
    bool isBeyondStop() const;                //
    void nextMotion();                      //
    float timeInMotionFloat{0.0F};            //

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    uint32_t sampleIndex{0};             // zero-based index of sample point inside the current motion
    float sampleZeroOffset{0.0F};        // time [s] between start of motion and first (index == 0) sample
    motionBuffer &theMotionBuffer;
};
