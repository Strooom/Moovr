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

#include "motion.h"

class motionBuffer {
  public:
    static constexpr uint8_t bufferLength{32};        // Length should be a compromise : large enough to allow good speed-optimization. Not too large to consume ram and cpu in optimizing
    motion motionBuffer[bufferLength];                // The buffer holding all motions to be executed
    bool isEmpty() const;                             //
    bool isFull() const;                              //
    uint32_t push();                                  // pushes 1 item on the buffer and returns an index to it..
    void pop();                                       // pops the oldest item on from the buffer and adjusts indexes and level
    motion* current();                             //
    uint32_t level();                        //
    uint32_t readIndex{0};             // First-to-be-executed or currently-being-executed motion
    void export2csv(const char* outputFilename);

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    uint32_t bufferLevel{0};           // number of motions in the buffer
    uint32_t bufferLevelMax{0};        // keeps track of maximum bufferlevel, as a help to dimension it
};
