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
#include "event.h"

class eventBuffer {
  public:
    eventBuffer();
    void pushEvent(event anEvent);                                 // push an event onto the eventBuffer
    event popEvent();                                              // pop an event from the eventbuffer
    bool hasEvents();                                              // check if there are any events
    uint32_t getBufferLevelMax();                                  // this allows to get the maximum number of items in the buffer, to help dimensioning it
    const char* toString(event anEvent) const;        // translate event code to string

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    static constexpr uint32_t bufferLength{32U};
    event theEventBuffer[bufferLength]{};
    uint32_t bufferReadIndex{0};
    uint32_t bufferLevel{0};
    uint32_t bufferLevelMax{0};
};
