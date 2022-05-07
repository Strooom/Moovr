// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

class gCodeWord {
  public:
    uint8_t letter{};            // gCode letter,eg 'G', 'M', 'X', etc
    double number{};             // value after the letter
    uint32_t intNumber{};        // for G or M letter words, contains number * 10, as integer , eg G38.1 holds 381, this simplifies lookup via switch statements
};
