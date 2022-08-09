// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class speedProfileCruise {
  public:
    speedProfileCruise();          //
    void set(float vMid);          //
    float a(float t) const;        // calculated acceleration at time t
    float v(float t) const;        // calculated speed at time t
    float s(float t) const;        // calculated distance travelled at time t
    float vMid{};                  // speed during the motion mid part [mm/s]
    float length{};                // total length of the motionPart [mm]
    float duration{};              // duration of motionPart [s]

    uint32_t toString(char *output) const;


  private:
};

