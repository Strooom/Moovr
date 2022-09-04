// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "event.h"
//#include "switches.h"

class debouncedInput {
  public:
    debouncedInput(event onOpen, event onClose);           // constructor
    void initialize(bool undebouncedInput);                // initialize to prevent transient events on startup
    event getEvent(bool undebouncedInput);                 // get press or release event
    bool getState() const;                                 // get current state of the input
    bool isClosed() const;                                 //
    bool isOpen() const;                                   //
    static constexpr uint32_t debounceMaxCount{4U};        // sets the upper boundary for debounceCounter
    const event onOpen;                                    // event to generate when this input opens
    const event onClose;                                   // event to generate when this input closes

  private:
    uint32_t debounceCounter{0U};        // counts up (input high) or down (input low) until it hits boundaries 0 or maxCount
    bool currentState{false};            // after debouncing : true : switch is closed / button pressed, false : switch open / button released
    bool previousState{false};           // remembers previous state, to detect flanks
};
