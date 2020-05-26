// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################


#include "MotionItemTrajectory.h"

void MotionItemTrajectory::print(const MotionType theType) const
    {
#ifndef  WIN32
    switch (theType)
        {
        case MotionType::Traverse:
        case MotionType::FeedLinear:
            Serial.println("  Linear Trajectory : ");
            Serial.print("    startPosition = ");
            for (uint8_t x = 0; x < nmbrAxis; x++)
                {
                Serial.print(startPosition[x]);
                Serial.print("  ");
                }
            Serial.println("mm");
            Serial.print("    positionChange = ");
            for (uint8_t x = 0; x < nmbrAxis; x++)
                {
                Serial.print(delta[x]);
                Serial.print("  ");
                }
            Serial.println("mm");

            Serial.print("    length = ");
            Serial.print(length);
            Serial.println(" mm");
            break;
        case MotionType::FeedHelicalCW:
        case MotionType::FeedHelicalCCW:
            Serial.println("  Helical Trajectory : ");
            Serial.print("    startPosition = ");
            for (uint8_t x = 0; x < nmbrAxis; x++)
                {
                Serial.print(startPosition[x]);
                Serial.print("  ");
                }
            Serial.println("mm");

            Serial.print("    arcCenter = ");
            Serial.print(arcCenter0);
            Serial.print("  ");
            Serial.print(arcCenter1);
            Serial.println("mm");

            Serial.print("    radius = ");
            Serial.print(radius);
            Serial.println(" mm");
            Serial.print("    startAngle = ");
            Serial.print(startAngle);
            Serial.println(" rad");
            Serial.print("    endAngle = ");
            Serial.print(startAngle + deltaAngle);
            Serial.println(" rad");
            Serial.print("    length = ");
            Serial.print(length);
            Serial.println(" mm");
            break;
        case MotionType::PauseAndResume:
        case MotionType::Pause:
            Serial.println("  Empty Trajectory : ");
            break;
        default:
            Serial.println("  Unknown Trajectory");
            break;
        }
#endif
    }

void MotionItemTrajectory::set(const gCodeParserResult &theParseResult)
    {
	// Todo : handle this depending on the motion type...
	// Lets set to zero all vars not used for a particular case, as this makes debugging cleaner..
    arcAxis0 = theParseResult.motion.trajectory.arcAxis[0];
    arcAxis1 = theParseResult.motion.trajectory.arcAxis[1];
    arcCenter0 = static_cast<float>(theParseResult.motion.trajectory.arcCenter[0]);
    arcCenter1 = static_cast<float>(theParseResult.motion.trajectory.arcCenter[1]);

    startAngle = static_cast<float>(theParseResult.motion.trajectory.startAngle);
    radius = static_cast<float>(theParseResult.motion.trajectory.radius);

    length = static_cast<float>(theParseResult.motion.trajectory.length);

    for (uint8_t i = 0; i < nmbrAxis; ++i)
        {
        if ((i == (uint8_t) theParseResult.motion.trajectory.arcAxis[0]) || (i == (uint8_t) theParseResult.motion.trajectory.arcAxis[1]))
            {
            deltaRealTime[i] = static_cast<float>(theParseResult.motion.trajectory.deltaAngle / theParseResult.motion.trajectory.length);					// for the ARC part
            directionUnitVector[i] = 1.0;																							// ### For circular moving Axis, we don't know the fraction, so we assume worst case
            }
        else
            {
            deltaRealTime[i] = static_cast<float>(theParseResult.motion.trajectory.delta[i] / theParseResult.motion.trajectory.length);					// for the LINEAR part
            directionUnitVector[i] = static_cast<float>(theParseResult.motion.trajectory.delta[i] / theParseResult.motion.trajectory.length);			// ### For linear moving Axis, take into account that this Axis is doing only a fraction of the speed of the 3d-move - the projection part..
            }
        startPosition[i] = static_cast<float>(theParseResult.motion.trajectory.startPosition[i]);
        delta[i] = static_cast<float>(theParseResult.motion.trajectory.delta[i]);
        }
    }
