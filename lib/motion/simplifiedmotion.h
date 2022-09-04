// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#pragma once
#include <stdint.h>
#include <limits>
#include "axis.h"
#include "motiontype.h"
#include "rotationdirection.h"
#include "point.h"

class simplifiedMotion {
  public:
    motionType type{motionType::none};

    struct Trajectory {
        double length{0.0};                                     // length of the motion [mm] - either as linear or as helical
        double startPosition[nmbrAxis]{0.0};                    // [mm], per axis
        double delta[nmbrAxis]{0.0};                            // delta = nextPosition - currentPosition [mm], per axis
        axis arcAxis[2]{axis::nmbrAxis, axis::nmbrAxis};        // which Axis forms the plane for the ARC
        double arcCenter[2]{0.0};                               // [mm]
        double radius{0.0};                                     // [mm]
        double startAngle{0.0};                                 // [rad]
        double deltaAngle{0.0};                                 // [rad]
    } trajectory;

    struct SpeedProfile {
        double vFeed{std::numeric_limits<double>::infinity()};        // speed requested by gCode [mm/s]
        double duration{0.0};                                         // duration [s]
    } speedProfile;

    struct Peripherals {
        bool coolantFlood{false};                                                //
        bool coolantMist{false};                                                 //
        double spindleSpeed{0.0};                                                //
        rotationDirection spindledirection{rotationDirection::clockwise};        //
    } peripherals;

    uint32_t toString(char* destBuffer);
    void setForTest(uint32_t aSet);
    void setForTest(motionType theType, uint32_t trajectoryIndex, uint32_t speedprofileIndex);
    void set(point &aPosition, axis anAxis, double offset, double speed);

    //void setRelative(point &aPosition, axis anAxis, double offset, double speed);
    //void setAbsolute(point &aPosition, axis anAxis, double offset, double speed);

};