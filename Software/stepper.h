// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include <stdint.h>
#include "machineproperties.h"
#include "step.h"

class stepBuffer {
  public:
    stepBuffer();
    void initialize();
    void write(step aStep);
    step read();
    uint32_t getBufferTimeInTicks() const;
    uint32_t getBufferLevel() const;
    bool needsFilling() const;

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    static constexpr uint32_t bufferLength = 256;        //
    step buffer[bufferLength];                           //
    uint32_t bufferLevel;                                //
    static constexpr uint32_t minBufferLevel = 4;        // number of ITEMS we want AT LEAST in the buffer
    uint32_t bufferHead;                                 //
    uint32_t bufferTimeInTicks;                          // this keeps track of the total timeBefore off all items in the buffer
    static constexpr uint32_t defaultReloadTime    = minStepBufferTotalTimeTicks / minBufferLevel;
    static constexpr uint32_t defaultOutputSignals = 0x00;
};

// -----------------------------------------------------------------------
// buffer parameter estimations (Teensy 3.5 / 3.6)
// -----------------------------------------------------------------------

// Example 1: max stepping frequency = 10k steps/s
// Oversampling x8 -> 80k samples/s
// 60M (PIT timer clock) / 80K = 750
// idle buffer should contain 4 actions, for a total timeBefore of ~10ms -> each item 2.5ms
// 2.5ms * 80K = 200 default reload value

// Example 2: max stepping frequency = 50k steps/s
// Oversampling x8 -> 400k samples/s
// 60M / 400K = 150
// reload = 1000
