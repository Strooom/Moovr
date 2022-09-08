// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "event.h"

class eventBuffer {
  public:
    void initialize();
    void pushEvent(event anEvent);
    event popEvent();
    bool hasEvents();
    uint32_t getMaxLevel();
    event getLastError();

#ifndef unitTesting
  private:
#endif
    static constexpr uint32_t length{16U};
    event theEventBuffer[eventBuffer::length]{};
    uint32_t head{0};
    uint32_t level{0};
    uint32_t maxLevel{0};
    event lastError{event::none};        // this keeps track of underflow or overflow errors
};
