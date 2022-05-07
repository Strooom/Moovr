// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>

enum class gCodeParseResultType : uint32_t {
    EmptyBlock,
    OkContextUpdateOnly,
    OkContextUpdateAndMotion,
    WarningContextUpdateOnly,
    WarningContextUpdateAndMotion,
    Error
};

const char* toString(gCodeParseResultType aParseResultType);