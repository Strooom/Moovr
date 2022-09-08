// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "motionitem.h"
#include "event.h"

class motionBuffer {
  public:
    bool isEmpty() const;             //
    bool isFull() const;              //
    uint32_t getLevel() const;        //
    uint32_t getMaxLevel();           //
    uint32_t push();                  // pushes 1 item on the buffer and returns an index to it..
    void pop();                       // pops the oldest item on from the buffer and adjusts indexes and level
    motion& getHead();                // returns a reference to the first motion to be / being executed
    void flush();                     // empties the motionBuffer, when you want to discard all current contents
    event getLastError();

#ifndef unitTesting
  private:
#endif
    friend class motionCtrl;
    static constexpr uint8_t length{32};              // Length should be a compromise : large enough to allow good speed-optimization. Not too large to consume ram and cpu in optimizing
    motion motionBuffer[motionBuffer::length];        // The buffer holding all motions to be executed
    uint32_t head{0};                                 // First-to-be-executed or currently-being-executed motion
    uint32_t level{0};                                // number of motions in the buffer
    uint32_t maxLevel{0};                             // keeps track of maximum bufferlevel, as a help to dimension it
    event lastError{event::none};                     // last error, if any, that happened in this buffer
};
