// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "motiontrajectory.h"
#include "motiontype.h"
#include "simplifiedmotion.h"
#include <stdio.h>

uint32_t motionTrajectory::toString(char* output, const motionType type) const {
    uint32_t outputLenght{0};

    switch (type) {
        case motionType::traverse:
        case motionType::feedLinear:
            outputLenght += sprintf(output + outputLenght, " Linear Trajectory : \n");
            outputLenght += sprintf(output + outputLenght, "  startPosition = ");
            for (uint8_t x = 0; x < nmbrAxis; x++) {
                outputLenght += sprintf(output + outputLenght, "%f ", startPosition[x]);
            }
            outputLenght += sprintf(output + outputLenght, "mm\n");

            outputLenght += sprintf(output + outputLenght, "  positionChange = ");
            for (uint8_t x = 0; x < nmbrAxis; x++) {
                outputLenght += sprintf(output + outputLenght, "%f ", delta[x]);
            }
            outputLenght += sprintf(output + outputLenght, "mm\n");
            outputLenght += sprintf(output + outputLenght, "  length =  %f  mm\n", length);

            break;
        case motionType::feedHelicalCW:
        case motionType::feedHelicalCCW:
            outputLenght += sprintf(output + outputLenght, " Helical Trajectory : \n");
            outputLenght += sprintf(output + outputLenght, "  startPosition = ");
            for (uint8_t x = 0; x < nmbrAxis; x++) {
                outputLenght += sprintf(output + outputLenght, "%f ", startPosition[x]);
            }
            outputLenght += sprintf(output + outputLenght, "mm\n");

            outputLenght += sprintf(output + outputLenght, "  arcCenter = %f %f mm\n", arcCenter0, arcCenter1);
            outputLenght += sprintf(output + outputLenght, "  radius = %f mm, startAngle =  %f rad, endAngle =  %f rad\n", radius, startAngle, startAngle + deltaAngle);
            outputLenght += sprintf(output + outputLenght, "  length =  %f  mm\n", length);
            break;
        case motionType::pauseAndResume:
        case motionType::pause:
            outputLenght += sprintf(output + outputLenght, " Empty Trajectory : \n");
            break;
        default:
            outputLenght += sprintf(output + outputLenght, " Unknown Trajectory : \n");
            break;
    }
    return outputLenght;
}

void motionTrajectory::set(const simplifiedMotion &theMotion) {
    // TODO : handle this depending on the motion type...
    // Lets set to zero all vars not used for a particular case, as this makes debugging cleaner..
    arcAxis0   = theMotion.trajectory.arcAxis[0];
    arcAxis1   = theMotion.trajectory.arcAxis[1];
    arcCenter0 = static_cast<float>(theMotion.trajectory.arcCenter[0]);
    arcCenter1 = static_cast<float>(theMotion.trajectory.arcCenter[1]);

    startAngle = static_cast<float>(theMotion.trajectory.startAngle);
    radius     = static_cast<float>(theMotion.trajectory.radius);

    length = static_cast<float>(theMotion.trajectory.length);

// TODO : how to handle here length = 0.0F ?? Apparently this does not lead to an error in floating point..

    for (uint8_t i = 0; i < nmbrAxis; ++i) {
        if ((i == (uint8_t)theMotion.trajectory.arcAxis[0]) || (i == (uint8_t)theMotion.trajectory.arcAxis[1])) {
            deltaRealTime[i]       = static_cast<float>(theMotion.trajectory.deltaAngle / theMotion.trajectory.length);        // for the ARC part
            directionUnitVector[i] = 1.0;                                                                                                              // ### For circular moving Axis, we don't know the fraction, so we assume worst case
        } else {
            deltaRealTime[i]       = static_cast<float>(theMotion.trajectory.delta[i] / theMotion.trajectory.length);        // for the LINEAR part
            directionUnitVector[i] = static_cast<float>(theMotion.trajectory.delta[i] / theMotion.trajectory.length);        // ### For linear moving Axis, take into account that this Axis is doing only a fraction of the speed of the 3d-move - the projection part..
        }
        startPosition[i] = static_cast<float>(theMotion.trajectory.startPosition[i]);
        delta[i]         = static_cast<float>(theMotion.trajectory.delta[i]);
    }
}
