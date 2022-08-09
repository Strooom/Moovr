// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "rotationdirection.h"
#include "simplifiedmotion.h"

class motionPeripherals {
  public:
    void set(const simplifiedMotion &theMotion);
    uint32_t toString(char *output) const;

#ifndef unitTesting
  private:
#endif
    friend class motion;
    friend class motionBuffer;
    friend class motionCtrl;

    rotationDirection spindledirection{rotationDirection::clockwise};
    float spindleSpeed{};
    bool coolantFlood{};
    bool coolantMist{};
};
