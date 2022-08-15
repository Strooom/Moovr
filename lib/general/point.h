// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "axis.h"

class point {
  public:
    float inMm[nmbrAxis]{0.0F};
    int32_t inSteps[nmbrAxis]{0}; // caution, SIGNED int, as this could go negative..

    void toString() const;
  private:
};
