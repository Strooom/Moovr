// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include <stdint.h>
#include "step.h"
#include "event.h"

class stepBuffer {
  public:
    stepBuffer(uint32_t lowWaterTotalTime, uint32_t lowWaterMark);
    void initialize();
    void write(step aStep);
    step read();
    bool needsFilling() const;

    uint32_t getTimeInTicks() const;
    uint32_t getLevel() const;
    uint32_t getMinLevel();
    uint32_t getMaxLevel();
    event getLastError();

#ifndef unitTesting
  private:
#endif
    static constexpr uint32_t length{256};        //
    step buffer[stepBuffer::length];              //
    uint32_t level{0};                            //
    uint32_t minLevel{length};                    // keeping track of lowest level of buffer, in order to help dimensioning machine parameters
    uint32_t maxLevel{0};                         // keeping track of highest level of buffer, in order to help dimensioning it's size
    uint32_t head{0};                             //
    uint32_t totalTime{0};                        // this keeps track of the total timeBefore off all items in the buffer

    uint32_t lowWaterTotalTime;        // minimum totalTime we'd like to have in the buffer
    uint32_t lowWaterMark;             // minimum number of items we'd like to have in the buffer

    event lastError{event::none};        // this keeps track of underflow or overflow errors
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
