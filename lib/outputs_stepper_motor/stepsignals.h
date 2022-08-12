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
    bool isModified();                                            // are the stepperSignals changed versus the last signals we sent out to the motors ?
    void next();                                                  // advance (discrete) time one tick
    void stepBackward(uint32_t axis);                             // add signals for setting a step forward on an axis
    void stepForward(uint32_t axis);                              // add signals for setting a step backward on an axis
    step output();                                                // gets a step after something is modified or old value if timeout
    void setMaxTicksSinceLastOutput(uint32_t newMaxTicks);        // set maxTicksSinceLastOutput
    bool isTimedOut();                                            // is there a certain (discrete) time passed since previous output ?

#ifndef unitTesting
  private:
#endif
    uint32_t dirSetup{0};                       // direction setup time
    uint32_t stepRise{0};                       // rising edge of step pulse
    uint32_t stepFall{0};                       // falling edge of step pulse
    uint32_t lastOutput{0};                     // last data sent to motor outputs
    uint32_t ticksSinceLastOutput{0};           // (discrete) time ticks since signals were sent to motor outputs
    uint32_t maxTicksSinceLastOutput{0};        // maximum (discrete) time ticks before sending signals to motor outputs
};
