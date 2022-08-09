// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class stepperMotorOutputs {
  public:
    void initialize();
    void write(uint32_t value);
    void enableMotors123(bool onOff);
    void enableMotors456(bool onOff);
};
