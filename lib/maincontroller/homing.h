// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include <stdint.h>
#include "event.h"
#include "homingstate.h"
#include "axis.h"

class homingController {
  public:
    void start();
    bool nextAxis();
    void handleEvents(event theEvent);        //
    void handleTimeouts();                    //

#ifndef unitTesting
  private:
#endif
    void goTo(homingState theNewHomingState);        // transitions in the stateMachine
    void enterState(homingState theNewState);        // actions when entering a state
    void exitState(homingState theOldState);         // actions when leaving a state

    bool selectAxis();        // true if homeable axis is selected, false if no homeable axis found

    homingState theHomingState{homingState::lost};        // current state of the homing process for selected axis
    uint32_t homingSequenceIndex{0};                          // index into machineProperties.homingSequence[] : 0 for 1st axis homing (eg. Z), 1 for second axis homing...
    axis currentHomingAxis{axis::nmbrAxis};               // value of machineProperties.homingSequence[homingAxisIndex]
    uint32_t currentHomingAxisIndex{nmbrAxis};            // same as currentHomingAxis but as integer value (io enum) so we can use it to index arrays
    uint32_t theLimitSwitch{0};                           // index into myInputs[] telling which input is the matching limit switch

    event limitSwitchClose{event::none};        // to which event should we listen for closing the limitswitch for selected axis
    event limitSwitchOpen{event::none};         // to which event should we listen for opening the limitswitch for selected axis

    // singleTimer timeOut;        // used to put a timeout on homing movements, eg, when the axis would not move due to a hw problem..
    //  TODO : how to set this timeout to a meaninfull value, should be calculated from machineProperties...
};
