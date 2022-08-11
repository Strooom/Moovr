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
    void pushEvent(event anEvent);        // push an event onto the eventBuffer
    event popEvent();                     // pop an event from the eventbuffer
    bool hasEvents();                     // check if there are any events
    uint32_t getLevelMax();               // this allows to get the maximum number of items in the buffer, to help dimensioning it
    event getLastError();

#ifndef unitTesting
  private:
#endif
    static constexpr uint32_t length{32U};
    event theEventBuffer[eventBuffer::length]{};
    uint32_t head{0};
    uint32_t level{0};
    uint32_t levelMax{0};
    event lastError{event::none};        // this keeps track of underflow or overflow errors
};
