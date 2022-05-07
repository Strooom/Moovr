// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "event.h"
#include "switches.h"

class debouncedInput {
  public:
    debouncedInput(inputs &someInputs, uint32_t index, event onOpen, event onClose);        // constructor
    bool getState() const;                                                                  // get current state of the input
    event getEvent();                                                                       // get press or release event
    static constexpr uint32_t debounceMaxCount{4U};        // sets the upper boundary for debounceCounter : 4 * 5ms = 20ms


  private:
    inputs &theInputs;           // reference to the HW inputs
    const uint32_t index;        // index into one of the bits of (HW) 'inputs' object
    const event onOpen;          // event to generate when this input opens
    const event onClose;         // event to generate when this input closes

    uint32_t debounceCounter{0U};                          // counts up (input high) or down (input low) until it hits boundaries 0 or maxCount
    bool currentState{false};                              // after debouncing : true : button is pressed, false : button is not pressed
    bool previousState{false};                             // remembers previous state, to detect flanks
};
