// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "step.h"

class stepSignals {
  public:
    bool isModified();                       //
    void next();                             //
    void stepBackward(uint32_t axis);        //
    void stepForward(uint32_t axis);         //
    step output();                           //
    step outputDefault();                    //
    bool isTimedOut();                       //

  private:
    uint32_t dirSetup{0};                    // direction setup time
    uint32_t stepRise{0};                    // rising edge of step pulse
    uint32_t stepFall{0};                    // falling edge of step pulse
    uint32_t lastOutput{0};                  // last data sent to motor outputs
    uint32_t ticksSinceLastOutput{0};        //
};
