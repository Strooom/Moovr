// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#ifndef  WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "general.h"										// required for definition of MotionSpeedProfile
#include "gcodeparseresult.h"								// required as gCodeParseResult is passed as a parameter to some memberfunctions


class motionPeripherals
    {
	public:
		void print() const;
		void set(const gCodeParserResult &theParseResult);
		
		rotationDirection spindledirection;
        float spindleSpeed;
        bool coolantFlood;
        bool coolantMist;
    };
