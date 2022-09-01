// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include "axis.h"

class positionInFloat {
  public:
    float coordinate[nmbrAxis];
};


class positionInDouble {
  public:
    double coordinate[nmbrAxis];
};


class positionInInteger {
  public:
    int32_t coordinate[nmbrAxis];
};