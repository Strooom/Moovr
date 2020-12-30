// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author : Pascal Roobrouck - @strooom                                  ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once

#ifndef  WIN32
#include <Arduino.h>
#else
#include <stdint.h>
#endif
#include "general.h"

class gCodeParserResult
    {
    public:
        enum class ParseResultType : uint8_t
            {
            EmptyBlock,						// empty GCode block - no errors, no
            OkContextUpdateOnly,
            OkContextUpdateAndMotion,
            WarningContextUpdateOnly,
            WarningContextUpdateAndMotion,
            Error,
            nmbrResults
            } theParseResultType;
        struct Motion
            {
            motionType theMotionType;
            struct Trajectory
                {
                double length;										// length of the motion [mm] - either as linear or as helical

                // for linear
                double startPosition[nmbrAxis];						// [mm], per axis
                double delta[nmbrAxis];								// delta = nextPosition - currentPosition [mm], per axis

                // for helical
                axis arcAxis[2];									// which Axis forms the plane for the ARC
                double arcCenter[2];								// [mm]
                double radius;										// [mm]
                double startAngle;									// [rad]
                double deltaAngle;									// [rad]
                } trajectory;										// struct holding all GCODE trajectory parameters, to be passed to Motion
            struct SpeedProfile
                {
                double vFeed;										// speed requested by gCode [mm/s]
                double duration;									// duration [s]
                } speedProfile;										// struct holding all GCODE speed/duration parameters, to be passed to Motion
            struct Peripherals
                {
                bool coolantFlood;
                bool coolantMist;
                double spindleSpeed;
                rotationDirection spindledirection;
                } peripherals;
            } motion;
        enum class Error : uint8_t
            {
            None = 0,
            TooManyWordsInBlock,
            InvalidArcParameters,
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

            MissingAxisWord,
            MissingOffsetWord,

            UnsupportedgCodeWord,

            MOTION_BUFFER_OVERFLOW,

            SoftLimits,

            ConfigFileError,

            nmbrErrors
            } error;

        void print(char* destBuffer);
    };