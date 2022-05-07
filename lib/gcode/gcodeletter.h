// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class gCodeLetter : uint8_t
    {
    //	All AXIS letters are put at the top, in the SAME order as in 'axis'. This way we can use loops to copy gCode letterValues to axis..
    X, Y, Z,
    //	A, B, C,
    //	D, E,
    //	U, V, W,

    //	After the AXIS letters, I put the OFFSET letters, so we can easily loop over those as well..
    I, J, K,
    L,
    F,
    //	G, H,
    //	M,
    N,
    //	O,
    P,
    //	Q,
    R, S, T,
    nmbrLetters
    };

constexpr uint32_t nmbrLetters = static_cast<uint32_t>(gCodeLetter::nmbrLetters);
