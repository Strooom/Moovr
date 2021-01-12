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
#include "step.h"

class stepSignals {
  public:
    bool isModified();                      //
    void next();                            //
    void stepBackward(uint8_t axis);        //
    void stepForward(uint8_t axis);         //
    step output();                          //
    step outputDefault();                   //
    bool isTimedOut();                      //

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    uint32_t dirSetup{0};                    // direction setup time
    uint32_t stepRise{0};                    // rising edge of step pulse
    uint32_t stepFall{0};                    // falling edge of step pulse
    uint32_t lastOutput{0};                  // last data sent to motor outputs
    uint32_t ticksSinceLastOutput{0};        //
};
