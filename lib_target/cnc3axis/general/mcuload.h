// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#include <limits>
#include <stdint.h>
#include "runtimer.h"

class mcuLoad {
  public:
    void start();
    void run();
    float getLoad();
    float getMaxLoad();

  private:
    intervalTimer sampleTimer;
    uint32_t loopCounter{0};
    float load{0.0F};
    float maxLoad{0.0F};
};