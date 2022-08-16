// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class gCodeParseError : uint32_t {
    None = 0,
    TooManyWordsInBlock, // more than gCodeBlock::MaxGCodeWordsPerGCodeBlock
    ModalGroupCollision,
    ValueWordCollision,
    InvalidLineNumber,
    InvalidFeedrate,
    MissingFeedrate,
    InvalidSpindleSpeed,

    MissingPForG4,
    InvalidDwellTime,

    MissingLForG10,
    MissingPForG10,
    InvalidPForG10,

    MissingAxisWord, // G0 or G1 without any X, Y or Z
    MissingOffsetWord, // G2 or G3 without 
    InvalidArcParameters,
    UnsupportedgCodeWord,
    SoftLimits,
    ConfigFileError,

    nmbrErrors
};

const char* toString(gCodeParseError aParseError);