// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "simplifiedmotion.h"
#include <stdio.h>        // needed for sprintf()
#include <limits>
#include <math.h>

uint32_t simplifiedMotion::toString(char* output) {
    uint32_t outputLenght{0};

    outputLenght += sprintf(output + outputLenght, "motionType = %s\n", ::toString(type));
    outputLenght += sprintf(output + outputLenght, "length = %f s\n", trajectory.length);
    // TODO : add all members in output
    // struct Trajectory {
    //     double length;                         // length of the motion [mm] - either as linear or as helical
    //     double startPosition[nmbrAxis];        // [mm], per axis
    //     double delta[nmbrAxis];                // delta = nextPosition - currentPosition [mm], per axis
    //     axis arcAxis[2];                       // which Axis forms the plane for the ARC
    //     double arcCenter[2];                   // [mm]
    //     double radius;                         // [mm]
    //     double startAngle;                     // [rad]
    //     double deltaAngle;                     // [rad]
    // } trajectory;                              //

    // struct SpeedProfile {
    //     double vFeed;           // speed requested by gCode [mm/s]
    //     double duration;        // duration [s]
    // } speedProfile;             //

    // struct Peripherals {
    //     bool coolantFlood;
    //     bool coolantMist;
    //     double spindleSpeed;
    //     rotationDirection spindledirection;
    // } peripherals;

    return outputLenght;
}

void simplifiedMotion::setForTest(uint32_t aSet) {
    switch (aSet) {
        case 0U:
        default:
            type                = motionType::traverse;
            trajectory.length   = 1.0f;
            trajectory.delta[0] = 1.0f;
            break;

        case 1U:
            type                = motionType::traverse;
            trajectory.length   = sqrt(3.0f);
            trajectory.delta[0] = 1.0f;
            trajectory.delta[1] = 1.0f;
            trajectory.delta[2] = 1.0f;
            break;

        case 2U:
            type                = motionType::traverse;
            trajectory.length   = 10.0f;
            trajectory.delta[0] = 10.0f;
            break;

        case 10U:
            type                = motionType::feedLinear;
            trajectory.length   = 1.0f;
            trajectory.delta[0] = 1.0f;
            speedProfile.vFeed  = 1.0f;
            break;

        case 20U:
            type               = motionType::feedHelicalCW;
            trajectory.length  = 1.0f;
            speedProfile.vFeed = 1.0f;
            break;

        case 30U:
            type               = motionType::feedHelicalCCW;
            trajectory.length  = 1.0f;
            speedProfile.vFeed = 1.0f;
            break;

        case 40U:
            type                  = motionType::pauseAndResume;
            speedProfile.duration = 1.0f;
            break;

        case 100U:
            type               = motionType::feedHelicalCW;
            trajectory.length  = 1.0f;
            trajectory.radius=1.0f;
            
            break;

        case 101U:
            type               = motionType::feedHelicalCCW;
            trajectory.length  = 1.0f;
            break;
    }
}

// void simplifiedMotion::setForTest(motionType theType, uint32_t trajectoryIndex, uint32_t speedprofileIndex) {
//     type = theType;
//     switch (trajectoryIndex) {
//         case 0U:
//             break;

//         default:
//             break;
//     }
//     switch (speedprofileIndex) {
//         case 0U:
//             break;

//         default:
//             break;
//     }
// }
