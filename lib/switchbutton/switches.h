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
    inputs();                        // constructor, configures the hardware
    bool get(uint32_t index);        // get a specific bit of inputs HAL-copy
    void sample();                   // read HW inputs into HAL-copy
    bool isReady() const;            //

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    static constexpr uint32_t nmbrInputs{12U};              // how many inputs does this particular HW have
#else
    static constexpr uint32_t nmbrInputs{12U};
#endif

#ifndef UnitTesting
  private:        // commented out during unit testing
#endif
    uint32_t theInputs{0};
    static constexpr uint32_t inputMask{0x00'00'00'00};        // this mask allows to invert individual bits in case of active low signals, io active high TODO : this should go to machineProperties
                                                               //    static constexpr uint32_t inputMask{0x00'00'0F'FF};        // this mask allows to invert individual bits in case of active low signals, io active high TODO : this should go to machineProperties
    bool ready{false};
};

// ---------------------------------------------------
// debounced input - mapped to IO and generates events
// ---------------------------------------------------
// Note : one instance per I/O

