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
#include "speedprofile.h"
#include "gcodeparseresult.h"								// required for access to gCodeParseResult, which is passed as parameter to member functions
#include "machineproperties.h"								// required for access to machineProperties, which is passed as parameter to member functions


class motionItemSpeedProfile
    {
	public:
		motionItemSpeedProfile();
		void print() const;
		void setSpeed(const gCodeParserResult &theParseResult);								// length and vFeed are inputs, and speedProfile is calculated
		void setDuration(const gCodeParserResult &theParseResult);							// duration is input, speed is zero

		SpeedProfileAccDec left;						// left part, usually the acceleration
        SpeedProfileCruise mid;							// mid part, the cruise
		SpeedProfileAccDec right;						// right part, usually the deceleration

        float vFeed;									// this is the (max) speed as 'wanted' by the GCode
        float vMax;										// this is the max speed as limited by machine constraints and overrides
        float aMax;										// this is the max acceleration as limited by machine constraints [mm/s^2], >=0
        float dMax;										// this is the max decelerationspeed as limited by machine constraints [mm/s^2], <=0
		float jMax;										// jMax is always as set in machineProperties and cannot be changed but is copied here to make things simpler
		float duration;									// [s] total duration of the motion


//		float tStop;									// time at which the motion will come to a stop - for feedhold.. if no stop needed, then this points (1.0 s) beyond tMotion, so the time > tStop will never assert
};