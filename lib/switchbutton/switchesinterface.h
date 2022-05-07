// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "event.h"

class switchesInterface {
  public:
    bool get(uint32_t index);                         // get a specific bit of inputs HAL-copy
    void sample();                                    // read HW inputs into HAL-copy
    bool isReady() const;                             //
    static constexpr uint32_t nmbrInputs{12U};        // how many inputs does this particular HW have

  private:
    uint32_t theInputs{0};
    static constexpr uint32_t inputMask{0x00'00'00'00};        // this mask allows to invert individual bits in case of active low signals, io active high TODO : this should go to machineProperties
    bool ready{false};
};
