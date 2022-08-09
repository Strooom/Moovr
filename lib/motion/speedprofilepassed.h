// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class speedProfilePassed {
  public:
    speedProfilePassed();                         //
    float length{};                               // total length of the motionPart [mm]
    float duration{};                             // duration of motionPart [s]
    uint32_t toString(char *output) const;        //

  private:
};