// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "event.h"

// -------------------------------
// raw GPIO - hardware abstraction
// -------------------------------
// Note : one object instance represents all needed I/Os
// 6 limit switches + 6 buttons

class inputs {
  public:
    static constexpr uint32_t nmbrInputs{12U};        // how many inputs does this particular HW have
    void initialize();                                // configures the hardware's GPIO registers
    bool get(uint32_t index);                         // get a specific bit of inputs HAL-copy
    void sample();                                    // read HW inputs into HAL-copy

  private:
    uint32_t theInputs{0};
    static constexpr uint32_t inputMask{0x00'00'00'00};        // this mask allows to invert individual bits in case of active low signals, io active high
    // TODO : this should go to machineProperties
};
