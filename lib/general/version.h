// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class version {
  public:
    explicit version(uint32_t a, uint32_t b, uint32_t c);
    const uint32_t mainVersion;
    const uint32_t subVersion;
    const uint32_t minorVersion;
    const char* toString();

  private:
    static constexpr int maxVersionStringLenght{14}; // "V xxx.yyy.zzz"
    char versionString[maxVersionStringLenght];
};
