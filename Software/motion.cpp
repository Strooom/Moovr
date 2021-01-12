#include "motion.h"
#include "gcodeparseresult.h"
#include "general.h"
#include "machineproperties.h"

#include <stdio.h>        // using sprintf
#include "math.h"         // using sqrtf, sinf and cosf

#ifdef WIN32
#include <fstream>
#include <iostream>
#endif

void motion::set(const gCodeParserResult &theParseResult, const machineProperties &theMachineProperties, const motionStrategy &theStrategy, const overrides &theOverrides) {
    type = theParseResult.motion.theMotionType;
    switch (type) {
        case motionType::Traverse:
        case motionType::FeedLinear:
        case motionType::FeedHelicalCW:
        case motionType::FeedHelicalCCW:
            // This case calculates from given length to a duration
            trajectory.set(theParseResult);               // copy Trajectory properties from parseResult into this motion
            limit(theMachineProperties);                  // limit the wanted speeds and accelerations to machine limitations for this trajectory
            speedProfile.setSpeed(theParseResult);        // copy SpeedProfile properties from parseResult into this motion
            optimize(theStrategy, theOverrides);
            peripherals.set(theParseResult);        // copy Peripheral properties
            break;
        case motionType::PauseAndResume:
        case motionType::Pause:
            // This case has duration given, and speeds are all zero
            trajectory.set(theParseResult);                  // trajectory is empty
            speedProfile.setDuration(theParseResult);        // duration is an input
            peripherals.set(theParseResult);
            break;
        case motionType::None:
            peripherals.set(theParseResult);
            break;
        default:
            break;
    }
}

float motion::vTri(motionStrategy strategy) const        // Calculate the maximum speed for a triangular profile
{
    if (motionStrategy::maximizeSpeed == strategy) {
        // For aMax > 0 and dMax < 0, the part under de sqrt is always positive
        return sqrtf(((speedProfile.right.vEnd * speedProfile.right.vEnd * speedProfile.aMax) - (speedProfile.left.vStart * speedProfile.left.vStart * speedProfile.dMax) - (2 * (trajectory.length - speedProfile.done.length) * speedProfile.aMax * speedProfile.dMax)) / (speedProfile.aMax - speedProfile.dMax));
    } else {
        float r = (2 * (trajectory.length - speedProfile.done.length) * speedProfile.aMax * speedProfile.dMax) - (speedProfile.right.vEnd * speedProfile.right.vEnd * speedProfile.dMax) + (speedProfile.left.vStart * speedProfile.left.vStart * speedProfile.aMax);
        if (r >= 0.0F) {
            return sqrtf(r / (speedProfile.aMax - speedProfile.dMax));
        } else {
            return 0.0F;
        }
    }
}

float motion::vTri(float vStart, float vEnd, float aMax, float dMax, float length, motionStrategy strategy) const {
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

void motion::optimizeCurrent(motionStrategy theStrategy, const overrides &theOverrides, float tNow) {
    speedProfile.left.setvStart(v(tNow));
    speedProfile.done.length   = s(tNow);
    speedProfile.done.duration = tNow;
    optimize(theStrategy, theOverrides);
}

void motion::optimizeOld(const motionStrategy theStrategy, const overrides &theOverrides, const machineProperties &theMachineProperties) {
    float vMid{0.0F};
    switch (type) {
        case motionType::Traverse:
        case motionType::FeedLinear:
        case motionType::FeedHelicalCW:
        case motionType::FeedHelicalCCW:
            if (theStrategy == motionStrategy::maximizeSpeed) {
                vMid = speedProfile.vFeed * theOverrides.feedOverride;        // vFeed was set in gCode, overrides may yield new value, but new vMid must still be within machine limits
                if (vMid > speedProfile.vMax) {
                    vMid = speedProfile.vMax;
                }
                speedProfile.left.setvEnd(vMid);
                speedProfile.right.setvStart(vMid);
                //speedProfile.left.calculate(motionSpeedProfileOrder::firstOrder);
                //speedProfile.right.calculate(motionSpeedProfileOrder::firstOrder);
                speedProfile.left.calculate(motionSpeedProfileOrder::secondOrder);
                speedProfile.right.calculate(motionSpeedProfileOrder::secondOrder);
                if ((speedProfile.left.length + speedProfile.right.length) <= (trajectory.length - speedProfile.done.length)) {
                    // Add a non-zero constant speed mid phase
                    speedProfile.mid.length = (trajectory.length - speedProfile.done.length) - (speedProfile.left.length + speedProfile.right.length);
                    speedProfile.mid.set(vMid);
                    speedProfile.mid.duration = speedProfile.mid.length / vMid;
                } else {
                    // Switch to a triangular T-profile
                    vMid = vTri(theStrategy);
                    speedProfile.left.setvEnd(vMid);
                    speedProfile.right.setvStart(vMid);
                    speedProfile.left.calculate(motionSpeedProfileOrder::firstOrder);
                    speedProfile.right.calculate(motionSpeedProfileOrder::firstOrder);
                    speedProfile.mid.length   = 0.0F;
                    speedProfile.mid.duration = 0.0F;
                }
                speedProfile.duration = speedProfile.done.duration + speedProfile.left.duration + speedProfile.mid.duration + speedProfile.right.duration;
                speedProfile.tStop    = std::numeric_limits<float>::infinity();
            } else {
                vMid = vTri(theStrategy);
                // TODO : if 0.0F can be reached, it is worthwhile to try second order profile for smoother stopping
                speedProfile.left.setvEnd(vMid);
                speedProfile.right.setvStart(vMid);
                //                speedProfile.left.calculate(motionSpeedProfileOrder::firstOrder);
                //                speedProfile.right.calculate(motionSpeedProfileOrder::firstOrder);
                speedProfile.left.calculate(motionSpeedProfileOrder::secondOrder);
                speedProfile.right.calculate(motionSpeedProfileOrder::secondOrder);
                speedProfile.mid.length   = 0.0F;
                speedProfile.mid.duration = 0.0F;
                speedProfile.duration     = speedProfile.done.duration + speedProfile.left.duration + speedProfile.mid.duration + speedProfile.right.duration;
                if (vMid == 0.0F) {
                    speedProfile.tStop = speedProfile.left.vStart / (-speedProfile.dMax);
                } else {
                    speedProfile.tStop = std::numeric_limits<float>::infinity();
                }
            }
            break;

        case motionType::PauseAndResume:
        case motionType::Pause:
        case motionType::None:
        default:
            break;
    }
}

void motion::optimize(const motionStrategy theStrategy, const overrides &theOverrides) {
    switch (type) {
        case motionType::Traverse:
        case motionType::FeedLinear:
        case motionType::FeedHelicalCW:
        case motionType::FeedHelicalCCW: {
            float vMid;
            motionSpeedProfileOrder order = motionSpeedProfileOrder::secondOrder;

            if (theStrategy == motionStrategy::maximizeSpeed) {
                vMid = speedProfile.vFeed * theOverrides.feedOverride;
                if (vMid > speedProfile.vMax) {
                    vMid = speedProfile.vMax;
                }
                float vMax = vTri(motionStrategy::maximizeSpeed);
                if (vMid > vMax) {
                    vMid  = vMax;
                    order = motionSpeedProfileOrder::firstOrder;
                }
            } else {
                vMid = 0.0F;
            }
            {
                float vMin = vTri(motionStrategy::minimizeSpeed);
                if (vMid < vMin) {
                    vMid  = vMin;
                    order = motionSpeedProfileOrder::firstOrder;
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
                order = motionSpeedProfileOrder::firstOrder;
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

        case motionType::PauseAndResume:
        case motionType::Pause:
        case motionType::None:
        default:
            break;
    }
}

float motion::vOut(motionStrategy strategy, motionCalculateDirection direction) const {
    float adMax;
    float vIn;

    if (motionCalculateDirection::forward == direction) {
        vIn = speedProfile.left.vStart;
        if (motionStrategy::maximizeSpeed == strategy) {
            adMax = speedProfile.aMax;
        } else {
            adMax = speedProfile.dMax;
        }
    } else {
        vIn = speedProfile.right.vEnd;
        if (motionStrategy::maximizeSpeed == strategy) {
            adMax = -1 * speedProfile.dMax;
        } else {
            adMax = -1 * speedProfile.aMax;
        }
    }

    float d;
    d = (vIn * vIn) + (2.0F * trajectory.length * adMax);
    if (d > 0.0F) {
        return sqrt(d);
    } else {
        return 0.0F;
    }
}

float motion::vOut(float vIn, float aMax, float dMax, float length, motionStrategy strategy, motionCalculateDirection direction) const {
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

void motion::limit(const machineProperties &theMachineProperties)        // calculate vMax, aMax and dMax from trajectory and machine limits
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

bool motion::isMoving(uint8_t axis) const {
    return trajectory.deltaRealTime[axis] != 0.0F;
}

uint32_t motion::toString(char *output) const {
    uint32_t outputLenght{0};
    outputLenght += sprintf(output + outputLenght, "motionType : ");
    switch (type) {
        case motionType::Traverse:
            outputLenght += sprintf(output + outputLenght, "Traverse\n");
            break;
        case motionType::FeedLinear:
            outputLenght += sprintf(output + outputLenght, "FeedLinear\n");
            break;
        case motionType::FeedHelicalCW:
            outputLenght += sprintf(output + outputLenght, "FeedHelicalCW\n");
            break;
        case motionType::FeedHelicalCCW:
            outputLenght += sprintf(output + outputLenght, "FeedHelicalCCW\n");
            break;
        case motionType::PauseAndResume:
            outputLenght += sprintf(output + outputLenght, "PauseAndResume\n");
            break;
        case motionType::Pause:
            outputLenght += sprintf(output + outputLenght, "Pause\n");
            break;
        default:
            outputLenght += sprintf(output + outputLenght, "unknown\n");
            break;
    }

    outputLenght += trajectory.toString(output + outputLenght, type);
    outputLenght += speedProfile.toString(output + outputLenght);
    outputLenght += peripherals.toString(output + outputLenght);

    return outputLenght;
}

void motion::export2csv(const char *outputFilename, uint32_t nmbrDataPoints) {
#ifdef WIN32
    //    std::ofstream outputFile("C:\\Users\\Pascal\\Google Drive\\motionOutput.csv");
    std::ofstream outputFile(outputFilename);
    outputFile << "time, acceleration, speed, distance" << std::endl;
    for (uint32_t i = 0; i <= nmbrDataPoints; i++) {
        float t = (speedProfile.duration * static_cast<float>(i)) / static_cast<float>(nmbrDataPoints);
        outputFile << t << "," << a(t) << "," << v(t) << "," << s(t) << std::endl;
    }
    outputFile.close();
#endif
}

void motion::setForTest(uint32_t setNmbr) {
    // This sets the motion parameters directly to a well defined value (whithout going through gCode parsing)
    // in order to simplify unit testing the motionController

    switch (setNmbr) {
        case 0U:
            type                        = motionType::FeedLinear;
            trajectory.delta[0]         = 1.0F;
            trajectory.length           = 1.0F;
            trajectory.deltaRealTime[0] = trajectory.delta[0] / trajectory.length;
            speedProfile.duration       = 1000.F * minStepBufferTotalTime;
            speedProfile.mid.vMid       = 1 / (1000.F * minStepBufferTotalTime);
            speedProfile.mid.duration   = 1000.F * minStepBufferTotalTime;

            break;
        case 1U:
            type                        = motionType::FeedLinear;
            trajectory.delta[0]         = 1.0F;
            trajectory.length           = 1.0F;
            trajectory.deltaRealTime[0] = trajectory.delta[0] / trajectory.length;
            speedProfile.duration       = minStepBufferTotalTime;
            speedProfile.mid.vMid       = 1 / minStepBufferTotalTime;
            speedProfile.mid.duration   = minStepBufferTotalTime;
            break;

        case 2U:
            type                  = motionType::FeedLinear;
            trajectory.length     = 1.0F;
            speedProfile.duration = minStepPulseWidth * 0.25F;
            //trajectory.delta[0]         = 1.0F;
            //trajectory.deltaRealTime[0] = trajectory.delta[0] / trajectory.length;
            //speedProfile.mid.vMid       = 1 / minStepBufferTotalTime;
            //speedProfile.mid.duration   = minStepBufferTotalTime;
            break;

        case 3U:
            type                  = motionType::FeedLinear;
            trajectory.length     = 1.0F;
            speedProfile.duration = minStepPulseWidth * 1.5F;
            //trajectory.delta[0]         = 1.0F;
            //trajectory.deltaRealTime[0] = trajectory.delta[0] / trajectory.length;
            //speedProfile.mid.vMid       = 1 / minStepBufferTotalTime;
            //speedProfile.mid.duration   = minStepBufferTotalTime;
            break;

        case 4U:
            type                  = motionType::FeedLinear;
            trajectory.length     = 1.0F;
            speedProfile.duration = minStepPulseWidth * 10.0F;
            speedProfile.tStop    = minStepPulseWidth * 2.5F;

            //trajectory.delta[0]         = 1.0F;
            //trajectory.deltaRealTime[0] = trajectory.delta[0] / trajectory.length;
            //speedProfile.mid.vMid       = 1 / minStepBufferTotalTime;
            //speedProfile.mid.duration   = minStepBufferTotalTime;
            break;

        default:
            break;
    }
}
