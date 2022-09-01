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
    bool isHomed() const;
    bool isHomed(axis theAxis) const;
    void handleEvents(event theEvent);               //
    void handleTimeouts();                           //
    void goTo(homingState theNewHomingState);        //
    void enterState(homingState theNewState);        //
    void exitState(homingState theOldState);         //

  private:
    homingState theHomingState{homingState::lost};        // current state of the homing process for selected axis
    void selectAxis();                                    //
    uint32_t axisIndex{0};                                // index to which axis we are currently homing
    uint32_t theLimitSwitch{0};                           // index into myInputs[] telling which input is the matching limit switch
    char gCodeCommand[16];                                // holds the piece of gcode to make the selected axis make the homing movement
};
