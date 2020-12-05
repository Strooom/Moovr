#pragma once

// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <Arduino.h>

class stepperMotorOutputs {
  public:
    stepperMotorOutputs();
    void enableMotors123(bool onOff);
    void enableMotors456(bool onOff);
};
