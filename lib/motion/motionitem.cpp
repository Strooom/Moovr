#include "motionitem.h"
#include "simplifiedmotion.h"
#include "machineproperties.h"

#include <stdio.h>        // using sprintf
#include "math.h"         // using sqrtf(), sinf() and cosf()
#include <limits>

extern machineProperties theMachineProperties;        // TODO : I think this is quite safe to be global

void motion::set(const simplifiedMotion &theMotion, overrides theOverrides, motionStrategy theStrategy) {
    type = theMotion.type;
    switch (type) {
            // This case calculates from given length to a duration
        case motionType::traverse:
        case motionType::feedLinear:
        case motionType::feedHelicalCW:
        case motionType::feedHelicalCCW:
            trajectory.set(theMotion);                  // copy Trajectory properties from parseResult into this motion
            limit();                                    // limit the wanted speeds and accelerations to machine limitations for this trajectory
            speedProfile.setSpeed(theMotion);           // copy SpeedProfile properties from parseResult into this motion
            optimize(theOverrides, theStrategy);        //
            peripherals.set(theMotion);                 // copy Peripheral properties
            break;
            // This case has duration given, and speeds are all zero
        case motionType::pauseAndResume:
        case motionType::pause:
            trajectory.set(theMotion);                  // trajectory is empty
            speedProfile.setDuration(theMotion);        // duration is an input
            peripherals.set(theMotion);
            break;
        case motionType::none:
            peripherals.set(theMotion);
            break;
        default:
            break;
    }
}

float motion::vTri(motionStrategy strategy) const        // Calculate the maximum speed for a triangular profile
{
    return vTri(speedProfile.left.vStart, speedProfile.right.vEnd, speedProfile.aMax, speedProfile.dMax, trajectory.length - speedProfile.done.length, strategy);

    // if (motionStrategy::maximizeSpeed == strategy) {
    //     // For aMax > 0 and dMax < 0, the part under de sqrt is always positive
    //     return sqrtf(((speedProfile.right.vEnd * speedProfile.right.vEnd * speedProfile.aMax) - (speedProfile.left.vStart * speedProfile.left.vStart * speedProfile.dMax) - (2 * (trajectory.length - speedProfile.done.length) * speedProfile.aMax * speedProfile.dMax)) / (speedProfile.aMax - speedProfile.dMax));
    // } else {
    //     float r = (2 * (trajectory.length - speedProfile.done.length) * speedProfile.aMax * speedProfile.dMax) - (speedProfile.right.vEnd * speedProfile.right.vEnd * speedProfile.dMax) + (speedProfile.left.vStart * speedProfile.left.vStart * speedProfile.aMax);
    //     if (r >= 0.0F) {
    //         return sqrtf(r / (speedProfile.aMax - speedProfile.dMax));
    //     } else {
    //         return 0.0F;
    //     }
    // }
}

float motion::vTri(float vStart, float vEnd, float aMax, float dMax, float length, motionStrategy strategy) {
    if (motionStrategy::maximizeSpeed == strategy) {
        // For aMax > 0 and dMax < 0, the part under de sqrt is always positive
        return sqrtf(((vEnd * vEnd * aMax) - (vStart * vStart * dMax) - (2 * length * aMax * dMax)) / (aMax - dMax));
    } else {
        float r = (2 * length * aMax * dMax) - (vEnd * vEnd * dMax) + (vStart * vStart * aMax);
        if (r >= 0.0F) {
            return sqrtf(r / (aMax - dMax));
        } else {
            return 0.0F;
        }
    }
}

float motion::s(float time) const {
    // TODO : could check for out of bounds values for time : <0 or >motion.duration
    if (time <= speedProfile.done.duration) {
        return speedProfile.done.length;
    }
    time -= speedProfile.done.duration;
    if (time <= speedProfile.left.duration) {
        return (speedProfile.done.length + speedProfile.left.s(time));
    }
    time -= speedProfile.left.duration;
    if (time <= speedProfile.mid.duration) {
        return (speedProfile.done.length + speedProfile.left.length + speedProfile.mid.s(time));
    }
    time -= speedProfile.mid.duration;
    return (speedProfile.done.length + speedProfile.left.length + speedProfile.mid.length + speedProfile.right.s(time));
}

float motion::v(float time) const {
    // TODO : could check for out of bounds values for time : <0 or >motion.duration
    if (time <= speedProfile.done.duration) {
        return speedProfile.left.v(0.0F);
    }
    time -= speedProfile.done.duration;
    if (time <= speedProfile.left.duration) {
        return speedProfile.left.v(time);
    }
    time -= speedProfile.left.duration;
    if (time <= speedProfile.mid.duration) {
        return speedProfile.mid.v(time);
    }
    time -= speedProfile.mid.duration;
    return speedProfile.right.v(time);
}

float motion::a(float time) const {
    // TODO : could check for out of bounds values for time : <0 or >motion.duration
    if (time <= speedProfile.done.duration) {
        return speedProfile.left.a(0.0F);
    }
    time -= speedProfile.done.duration;
    if (time <= speedProfile.left.duration) {
        return speedProfile.left.a(time);
    }
    time -= speedProfile.left.duration;
    if (time <= speedProfile.mid.duration) {
        return speedProfile.mid.a(time);
    }
    time -= speedProfile.mid.duration;
    return speedProfile.right.a(time);
}

void motion::optimize(overrides theOverrides, motionStrategy theStrategy, float tNow) {
    speedProfile.left.setvStart(v(tNow));
    speedProfile.done.length   = s(tNow);
    speedProfile.done.duration = tNow;
    optimize(theOverrides, theStrategy);
}

void motion::optimize(overrides theOverrides, motionStrategy theStrategy) {
    switch (type) {
        case motionType::traverse:
        case motionType::feedLinear:
        case motionType::feedHelicalCW:
        case motionType::feedHelicalCCW: {
            float vMid;
            speedProfileOrder order;

            if (trajectory.length < theMachineProperties.minLengthSProfile) {
                order = speedProfileOrder::firstOrder;
            } else {
                order = speedProfileOrder::secondOrder;
            }

            if (theStrategy == motionStrategy::maximizeSpeed) {
                vMid = speedProfile.vFeed * theOverrides.feedOverride;
                if (vMid > speedProfile.vMax) {
                    vMid = speedProfile.vMax;
                }
                float vMax = vTri(motionStrategy::maximizeSpeed);
                if (vMid > vMax) {
                    vMid  = vMax;
                    order = speedProfileOrder::firstOrder;
                }
            } else {
                vMid = 0.0F;
            }
            {
                float vMin = vTri(motionStrategy::minimizeSpeed);
                if (vMid < vMin) {
                    vMid  = vMin;
                    order = speedProfileOrder::firstOrder;
                }
            }
            speedProfile.left.setMax(speedProfile.aMax, speedProfile.dMax, speedProfile.jMax);
            speedProfile.left.setvEnd(vMid);
            speedProfile.left.calculate(order);
            speedProfile.right.setMax(speedProfile.aMax, speedProfile.dMax, speedProfile.jMax);
            speedProfile.right.setvStart(vMid);
            speedProfile.right.calculate(order);
            speedProfile.mid.set(vMid);

            if ((trajectory.length - speedProfile.done.length) < (speedProfile.left.length + speedProfile.right.length)) {
                order = speedProfileOrder::firstOrder;
                speedProfile.left.setMax(speedProfile.aMax, speedProfile.dMax, speedProfile.jMax);
                speedProfile.left.setvEnd(vMid);
                speedProfile.left.calculate(order);
                speedProfile.right.setMax(speedProfile.aMax, speedProfile.dMax, speedProfile.jMax);
                speedProfile.right.setvStart(vMid);
                speedProfile.right.calculate(order);
                speedProfile.mid.set(vMid);
            }

            speedProfile.mid.length   = (trajectory.length - speedProfile.done.length) - (speedProfile.left.length + speedProfile.right.length);
            speedProfile.mid.duration = speedProfile.mid.length / vMid;
            speedProfile.duration     = speedProfile.done.duration + speedProfile.left.duration + speedProfile.mid.duration + speedProfile.right.duration;

            if (vMid == 0.0F) {
                speedProfile.tStop = speedProfile.done.duration + speedProfile.left.duration;
            } else {
                speedProfile.tStop = std::numeric_limits<float>::infinity();
            }
        } break;

        case motionType::pauseAndResume:
        case motionType::pause:
        case motionType::none:
        default:
            break;
    }
}

float motion::vOut(motionStrategy strategy, motionCalculateDirection direction) const {
    if (motionCalculateDirection::forward == direction) {
        return vOut(speedProfile.left.vStart, speedProfile.aMax, speedProfile.dMax, trajectory.length, strategy, direction);
    } else {
        return vOut(speedProfile.right.vEnd, speedProfile.aMax, speedProfile.dMax, trajectory.length, strategy, direction);
    }
}

float motion::vOut(float vIn, float aMax, float dMax, float length, motionStrategy strategy, motionCalculateDirection direction) {
    float adMax;

    if (motionCalculateDirection::forward == direction) {
        if (motionStrategy::maximizeSpeed == strategy) {
            adMax = aMax;
        } else {
            adMax = dMax;
        }
    } else {
        if (motionStrategy::maximizeSpeed == strategy) {
            adMax = -1 * dMax;
        } else {
            adMax = -1 * aMax;
        }
    }

    float d;
    d = (vIn * vIn) + (2.0F * length * adMax);
    if (d > 0.0F) {
        return sqrt(d);
    } else {
        return 0.0F;
    }
}

void motion::limit()        // calculate vMax, aMax and dMax from trajectory and machine limits
{
    speedProfile.vMax = std::numeric_limits<float>::infinity();
    speedProfile.aMax = std::numeric_limits<float>::infinity();
    speedProfile.dMax = -std::numeric_limits<float>::infinity();

    for (uint8_t i = 0; i < nmbrAxis; ++i)        // iterate over all Axis
    {
        if ((i == (uint8_t)trajectory.arcAxis0) || (i == (uint8_t)trajectory.arcAxis1))        // for all Axis, except for the 2 of the arcPlane
        {
            if (speedProfile.vMax > sqrtf(theMachineProperties.motors.aMax[i] * trajectory.radius)) {
                speedProfile.vMax = static_cast<float>(sqrtf(theMachineProperties.motors.aMax[i] * trajectory.radius));
            }
        }

        if (speedProfile.vMax > fabs(theMachineProperties.motors.vMax[i] / trajectory.directionUnitVector[i]))        // it seems the floating point division understands 'infinity' and so this works without testing for div by zero
        {
            speedProfile.vMax = static_cast<float>(fabs(theMachineProperties.motors.vMax[i] / trajectory.directionUnitVector[i]));        // restrict vMax, if this would result in exceeding vMax for any of the Axis
        }
        if (speedProfile.aMax > fabs(theMachineProperties.motors.aMax[i] / trajectory.directionUnitVector[i])) {
            speedProfile.aMax = static_cast<float>(fabs(theMachineProperties.motors.aMax[i] / trajectory.directionUnitVector[i]));        // restrict aMax, if this would result in exceeding aMax for any of the Axis
        }
        if (fabs(speedProfile.dMax) > fabs(theMachineProperties.motors.dMax[i] / trajectory.directionUnitVector[i])) {
            speedProfile.dMax = static_cast<float>(-1 * fabs(theMachineProperties.motors.dMax[i] / trajectory.directionUnitVector[i]));        // restrict dMax, if this would result in exceeding dMax for any of the Axis
        }
    }
    speedProfile.jMax = theMachineProperties.motors.jMax;
}

bool motion::isMoving(uint32_t anAxis) const {
    return trajectory.deltaRealTime[anAxis] != 0.0F;
}

uint32_t motion::toString(char *output) const {
    uint32_t outputLenght{0};
    outputLenght += sprintf(output + outputLenght, "motionType : ");

    outputLenght += trajectory.toString(output + outputLenght, type);
    outputLenght += speedProfile.toString(output + outputLenght);
    outputLenght += peripherals.toString(output + outputLenght);

    return outputLenght;
}

void motion::positionFromDistance(point &aPosition, float s) {
    // TODO s is confusion as parameter as it is also the name of a member function
    for (uint32_t axisIndex = 0; axisIndex < nmbrAxis; axisIndex++) {
        if (isMoving(axisIndex)) {
            // first calculation the position in mm
            if (axisIndex == static_cast<uint32_t>(trajectory.arcAxis0)) {
                aPosition.inMm[axisIndex] = (trajectory.arcCenter0 + (trajectory.radius * cosf(trajectory.startAngle + (trajectory.deltaRealTime[axisIndex] * s))));
            } else if (axisIndex == static_cast<uint32_t>(trajectory.arcAxis1)) {
                aPosition.inMm[axisIndex] = (trajectory.arcCenter1 + (trajectory.radius * sinf(trajectory.startAngle + (trajectory.deltaRealTime[axisIndex] * s))));
            } else {
                aPosition.inMm[axisIndex] = (trajectory.startPosition[axisIndex] + trajectory.deltaRealTime[axisIndex] * s);
            }
            // then calculate position in steps
            aPosition.inSteps[axisIndex] = static_cast<int32_t>(aPosition.inMm[axisIndex] * theMachineProperties.motors.stepsPerMm[axisIndex]);
        }
    }
}

float motion::positionInMmFromDistanceTravelled(uint32_t anAxis, float distanceTravelled) {
    if (anAxis == static_cast<uint32_t>(trajectory.arcAxis0)) {
        return (trajectory.arcCenter0 + (trajectory.radius * cosf(trajectory.startAngle + (trajectory.deltaRealTime[anAxis] * distanceTravelled))));
    } else if (anAxis == static_cast<uint32_t>(trajectory.arcAxis1)) {
        return (trajectory.arcCenter1 + (trajectory.radius * sinf(trajectory.startAngle + (trajectory.deltaRealTime[anAxis] * distanceTravelled))));
    } else {
        return (trajectory.startPosition[anAxis] + trajectory.deltaRealTime[anAxis] * distanceTravelled);
    }
}

// void motion::export2csv(const char *outputFilename, uint32_t nmbrDataPoints) {
// // #ifdef WIN32
// //     //    std::ofstream outputFile("C:\\Users\\Pascal\\Google Drive\\motionOutput.csv");
// //     std::ofstream outputFile(outputFilename);
// //     outputFile << "time, acceleration, speed, distance" << std::endl;
// //     for (uint32_t i = 0; i <= nmbrDataPoints; i++) {
// //         float t = (speedProfile.duration * static_cast<float>(i)) / static_cast<float>(nmbrDataPoints);
// //         outputFile << t << "," << a(t) << "," << v(t) << "," << s(t) << std::endl;
// //     }
// //     outputFile.close();
// // #endif
// }

// void motion::setForTest(uint32_t setNmbr) {
//     // This sets the motion parameters directly to a well defined value (whithout going through gCode parsing)
//     // in order to simplify unit testing the motionController

//     switch (setNmbr) {
//         case 0U:
//             type                        = motionType::feedLinear;
//             trajectory.delta[0]         = 1.0F;
//             trajectory.length           = 1.0F;
//             trajectory.deltaRealTime[0] = trajectory.delta[0] / trajectory.length;
//             speedProfile.duration       = 1000.F * minStepBufferTotalTime;
//             speedProfile.mid.vMid       = 1 / (1000.F * minStepBufferTotalTime);
//             speedProfile.mid.duration   = 1000.F * minStepBufferTotalTime;

//             break;
//         case 1U:
//             type                        = motionType::feedLinear;
//             trajectory.delta[0]         = 1.0F;
//             trajectory.length           = 1.0F;
//             trajectory.deltaRealTime[0] = trajectory.delta[0] / trajectory.length;
//             speedProfile.duration       = minStepBufferTotalTime;
//             speedProfile.mid.vMid       = 1 / minStepBufferTotalTime;
//             speedProfile.mid.duration   = minStepBufferTotalTime;
//             break;

//         case 2U:
//             type                  = motionType::feedLinear;
//             trajectory.length     = 1.0F;
//             speedProfile.duration = minStepPulseWidth * 0.25F;
//             //trajectory.delta[0]         = 1.0F;
//             //trajectory.deltaRealTime[0] = trajectory.delta[0] / trajectory.length;
//             //speedProfile.mid.vMid       = 1 / minStepBufferTotalTime;
//             //speedProfile.mid.duration   = minStepBufferTotalTime;
//             break;

//         case 3U:
//             type                  = motionType::feedLinear;
//             trajectory.length     = 1.0F;
//             speedProfile.duration = minStepPulseWidth * 1.5F;
//             //trajectory.delta[0]         = 1.0F;
//             //trajectory.deltaRealTime[0] = trajectory.delta[0] / trajectory.length;
//             //speedProfile.mid.vMid       = 1 / minStepBufferTotalTime;
//             //speedProfile.mid.duration   = minStepBufferTotalTime;
//             break;

//         case 4U:
//             type                  = motionType::feedLinear;
//             trajectory.length     = 1.0F;
//             speedProfile.duration = minStepPulseWidth * 10.0F;
//             speedProfile.tStop    = minStepPulseWidth * 2.5F;

//             //trajectory.delta[0]         = 1.0F;
//             //trajectory.deltaRealTime[0] = trajectory.delta[0] / trajectory.length;
//             //speedProfile.mid.vMid       = 1 / minStepBufferTotalTime;
//             //speedProfile.mid.duration   = minStepBufferTotalTime;
//             break;

//         default:
//             break;
//     }
// }

// void motion::optimizeOld() {
//      float vMid{0.0F};
//      switch (type) {
//          case motionType::traverse:
//          case motionType::feedLinear:
//          case motionType::feedHelicalCW:
//          case motionType::feedHelicalCCW:
//              if (theStrategy == motionStrategy::maximizeSpeed) {
//                  vMid = speedProfile.vFeed * theOverrides.feedOverride;        // vFeed was set in gCode, overrides may yield new value, but new vMid must still be within machine limits
//                  if (vMid > speedProfile.vMax) {
//                      vMid = speedProfile.vMax;
//                  }
//                  speedProfile.left.setvEnd(vMid);
//                  speedProfile.right.setvStart(vMid);
//                  // speedProfile.left.calculate(motionSpeedProfileOrder::firstOrder);
//                  // speedProfile.right.calculate(motionSpeedProfileOrder::firstOrder);
//                  speedProfile.left.calculate(speedProfileOrder::secondOrder);
//                  speedProfile.right.calculate(speedProfileOrder::secondOrder);
//                  if ((speedProfile.left.length + speedProfile.right.length) <= (trajectory.length - speedProfile.done.length)) {
//                      // Add a non-zero constant speed mid phase
//                      speedProfile.mid.length = (trajectory.length - speedProfile.done.length) - (speedProfile.left.length + speedProfile.right.length);
//                      speedProfile.mid.set(vMid);
//                      speedProfile.mid.duration = speedProfile.mid.length / vMid;
//                  } else {
//                      // Switch to a triangular T-profile
//                      vMid = vTri(theStrategy);
//                      speedProfile.left.setvEnd(vMid);
//                      speedProfile.right.setvStart(vMid);
//                      speedProfile.left.calculate(speedProfileOrder::firstOrder);
//                      speedProfile.right.calculate(speedProfileOrder::firstOrder);
//                      speedProfile.mid.length   = 0.0F;
//                      speedProfile.mid.duration = 0.0F;
//                  }
//                  speedProfile.duration = speedProfile.done.duration + speedProfile.left.duration + speedProfile.mid.duration + speedProfile.right.duration;
//                  speedProfile.tStop    = std::numeric_limits<float>::infinity();
//              } else {
//                  vMid = vTri(theStrategy);
//                  // TODO : if 0.0F can be reached, it is worthwhile to try second order profile for smoother stopping
//                  speedProfile.left.setvEnd(vMid);
//                  speedProfile.right.setvStart(vMid);
//                  //                speedProfile.left.calculate(motionSpeedProfileOrder::firstOrder);
//                  //                speedProfile.right.calculate(motionSpeedProfileOrder::firstOrder);
//                  speedProfile.left.calculate(speedProfileOrder::secondOrder);
//                  speedProfile.right.calculate(speedProfileOrder::secondOrder);
//                  speedProfile.mid.length   = 0.0F;
//                  speedProfile.mid.duration = 0.0F;
//                  speedProfile.duration     = speedProfile.done.duration + speedProfile.left.duration + speedProfile.mid.duration + speedProfile.right.duration;
//                  if (vMid == 0.0F) {
//                      speedProfile.tStop = speedProfile.left.vStart / (-speedProfile.dMax);
//                  } else {
//                      speedProfile.tStop = std::numeric_limits<float>::infinity();
//                  }
//              }
//              break;

//         case motionType::pauseAndResume:
//         case motionType::pause:
//         case motionType::none:
//         default:
//             break;
//     }
// }
