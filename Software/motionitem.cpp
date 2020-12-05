#include "general.h"
#include "machineproperties.h"
#include "gcodeparseresult.h"
#include "motionitem.h"

void MotionItem::set(const gCodeParserResult &theParseResult, const machineProperties &theMachineProperties, const MotionStrategy &theStrategy, const overrides &theOverrides)
    {
    theType = theParseResult.motion.theMotionType;
    switch (theType)
        {
        case MotionType::Traverse:
        case MotionType::FeedLinear:
        case MotionType::FeedHelicalCW:
        case MotionType::FeedHelicalCCW:
            // This case calculates from given length to a duration
            theTrajectory.set(theParseResult);						// copy Trajectory properties from parseResult into this motionItem
            limit(theMachineProperties);							// limit the wanted speeds and accelerations to machine limitations for this trajectory
            theSpeedProfile.setSpeed(theParseResult);				// copy SpeedProfile properties from parseResult into this motionItem
            optimize(theStrategy, theOverrides);
            theSpeedProfile.duration = theSpeedProfile.left.duration + theSpeedProfile.mid.duration + theSpeedProfile.right.duration;
            thePeripherals.set(theParseResult);						// copy Peripheral properties
            break;
        case MotionType::PauseAndResume:
        case MotionType::Pause:
            // This case has duration given, and speeds are all zero
            theTrajectory.set(theParseResult);						// trajectory is empty
            theSpeedProfile.setDuration(theParseResult);			// duration is an input
            thePeripherals.set(theParseResult);
            break;
        case MotionType::None:
            thePeripherals.set(theParseResult);
            break;
        default:
            break;
        }
    }

float MotionItem::calcTriangular(MotionStrategy theStrategy)						// Calculate the maximum speed for a triangular profile
    {
    if (MotionStrategy::maximizeSpeed == theStrategy)
        {
        // For aMax > 0 and dMax < 0, the part under de sqrt is always positive
        return sqrtf(((theSpeedProfile.right.vEnd * theSpeedProfile.right.vEnd * theSpeedProfile.aMax) - (theSpeedProfile.left.vStart * theSpeedProfile.left.vStart * theSpeedProfile.dMax) -                       (2 * theTrajectory.length * theSpeedProfile.aMax * theSpeedProfile.dMax)) / (theSpeedProfile.aMax - theSpeedProfile.dMax));
        }
    else
        {
        float r = (2 * theTrajectory.length * theSpeedProfile.aMax * theSpeedProfile.dMax) - (theSpeedProfile.right.vEnd * theSpeedProfile.right.vEnd * theSpeedProfile.dMax) + (theSpeedProfile.left.vStart * theSpeedProfile.left.vStart * theSpeedProfile.aMax);
        if (r >= 0.0F)
            {
            return sqrtf(r / (theSpeedProfile.aMax - theSpeedProfile.dMax));
            }
        else
            {
            return 0.0F;
            }
        }
    }

float MotionItem::s(float time) const
    {
    if (time >= theSpeedProfile.left.duration)
        {
        return theSpeedProfile.left.s(time);
        }
    else if (time >= theSpeedProfile.left.duration + theSpeedProfile.mid.duration)
        {
        return (theSpeedProfile.left.length + theSpeedProfile.mid.s(time - theSpeedProfile.left.duration));
        }
    else
        {
        return (theSpeedProfile.left.length + theSpeedProfile.mid.length + theSpeedProfile.right.s(time - (theSpeedProfile.left.duration + theSpeedProfile.mid.duration)));
        }
    }

float MotionItem::v(float time) const
    {
    if (time >= theSpeedProfile.left.duration)
        {
        return theSpeedProfile.left.v(time);
        }
    else if (time >= theSpeedProfile.left.duration + theSpeedProfile.mid.duration)
        {
        return theSpeedProfile.mid.v(time - theSpeedProfile.left.duration);
        }
    else
        {
        return theSpeedProfile.right.v(time - (theSpeedProfile.left.duration + theSpeedProfile.mid.duration));
        }
    }

float MotionItem::a(float time) const
    {
    if (time >= theSpeedProfile.left.duration)
        {
        return theSpeedProfile.left.a(time);
        }
    else if (time >= theSpeedProfile.left.duration + theSpeedProfile.mid.duration)
        {
        return theSpeedProfile.mid.a(time - theSpeedProfile.left.duration);
        }
    else
        {
        return theSpeedProfile.right.a(time - (theSpeedProfile.left.duration + theSpeedProfile.mid.duration));
        }
    }

void MotionItem::optimize(MotionStrategy theStrategy, const overrides &theOverrides)			// optimizes this motionItem's speed profile according to current strategy and overrides
    {
    float vMid{ 0.0F };
    switch (theType)
        {
        case MotionType::Traverse:
        case MotionType::FeedLinear:
        case MotionType::FeedHelicalCW:
        case MotionType::FeedHelicalCCW:
            if (MotionStrategy::maximizeSpeed == theStrategy)
                {
                vMid = theSpeedProfile.vFeed * theOverrides.feedOverride;							// vFeed was set in gCode, overrides may yield new value, but new vMid must still be within machine limits
                if (vMid > theSpeedProfile.vMax)
                    {
                    vMid = theSpeedProfile.vMax;
                    }
                }
            theSpeedProfile.left.setvEnd(vMid);
            theSpeedProfile.right.setvStart(vMid);
            theSpeedProfile.left.calculate(MotionSpeedProfileOrder::secondOrder);
            theSpeedProfile.right.calculate(MotionSpeedProfileOrder::secondOrder);
            if ((theSpeedProfile.left.length + theSpeedProfile.right.length) > theTrajectory.length)
                {
                // Switch to a triangular T-profile
                vMid = calcTriangular(theStrategy);
                theSpeedProfile.left.setvEnd(vMid);
                theSpeedProfile.right.setvStart(vMid);
                theSpeedProfile.left.calculate(MotionSpeedProfileOrder::firstOrder);
                theSpeedProfile.right.calculate(MotionSpeedProfileOrder::firstOrder);
                theSpeedProfile.mid.length = 0.0F;
                theSpeedProfile.mid.duration = 0.0F;
                }
            else
                {
                // Add a non-zero constant speed mid phase
                theSpeedProfile.mid.length = theTrajectory.length - (theSpeedProfile.left.length + theSpeedProfile.right.length);
                theSpeedProfile.mid.set(vMid);
                theSpeedProfile.mid.duration = theSpeedProfile.mid.length / vMid;
                }


            break;
        case MotionType::PauseAndResume:
        case MotionType::Pause:
        case MotionType::None:
        default:

            break;
        }
    }

float MotionItem::calcOtherV(MotionStrategy theStrategy, bool forward)
    {
    // calculates the maximal / minimal entry/exit speed, given length (s), aMax or dMax (adMAx) and vStart or vEnd (v)
    // theStrategy : determines if we want to maximize or minimize the vOther
    // forward : determines if we have vStart as a given, calculating vEnd (true), or vEnd as a given, calculating vStart (false)

    //float adMax;	// holds aMax or dMax, depending on the strategy and forward/backward
    //float v;		// holds vStart or vEnd, depending on the forward/backward

    //if (forward)
    //    {
    //    v = vStart;
    //    }
    //else
    //    {
    //    v = vEnd;
    //    }

    //switch (theStrategy)
    //    {
    //    case MotionStrategy::minimizeSpeed:
    //        if (forward)
    //            {
    //            adMax = dMax;
    //            }
    //        else
    //            {
    //            adMax = -1 * aMax;
    //            }
    //        break;

    //    default:
    //    case MotionStrategy::maximizeSpeed:
    //        if (forward)
    //            {
    //            adMax = aMax;
    //            }
    //        else
    //            {
    //            adMax = -1 * dMax;
    //            }
    //        break;
    //    }

    //switch (theMotionProfile)
    //    {
    //    default:
    //    case MotionSpeedProfile::firstOrder:
    //        // in case we decelerate, will we reach vOther = 0 in less distance than s ??
    //        float d;
    //        d = (v * v) + (2.0F * sMotion * adMax);
    //        if (d > 0.0F)
    //            {
    //            return sqrt(d);
    //            }
    //        else
    //            {
    //            return 0.0F;
    //            }
    //        break;

    //    case MotionSpeedProfile::secondOrder:
    //        // Todo
    //        return 0.0F;
    //        break;
    //    }
    return 0.0F; //make compiler happy
    }

void MotionItem::adjustRemaining()
    {
    // adjust the currently being executed motionItem to its remaining part, so it can be optimized properly with a new speedprofile
    // TODO :
    // vStart = v(now())
    // length = lenght - s(now())
    // etc..
    // some things don't change, such as arcCenter, axis, vFeed, vMax, ...
    }

void MotionItem::limit(const machineProperties &theMachineProperties)			// calculate vMax, aMax and dMax from trajectory and machine limits
    {
    theSpeedProfile.vMax = largeValue;
    theSpeedProfile.aMax = largeValue;
    theSpeedProfile.dMax = -largeValue;

    for (uint8_t i = 0; i < nmbrAxis; ++i)																							// iterate over all Axis
        {
        if ((i == (uint8_t) theTrajectory.arcAxis0) || (i == (uint8_t) theTrajectory.arcAxis1))										// for all Axis, except for the 2 of the arcPlane
            {
            if (theSpeedProfile.vMax > sqrt(theMachineProperties.motors.aMax[i] * theTrajectory.radius))
                {
                theSpeedProfile.vMax = static_cast<float>(sqrt(theMachineProperties.motors.aMax[i] * theTrajectory.radius));
                }
            }

        if (theSpeedProfile.vMax > fabs(theMachineProperties.motors.vMax[i] / theTrajectory.directionUnitVector[i]))								// it seems the floating point division understands 'infinity' and so this works without testing for div by zero
            {
            theSpeedProfile.vMax = static_cast<float>(fabs(theMachineProperties.motors.vMax[i] / theTrajectory.directionUnitVector[i]));			// restrict vMax, if this would result in exceeding vMax for any of the Axis
            }
        if (theSpeedProfile.aMax > fabs(theMachineProperties.motors.aMax[i] / theTrajectory.directionUnitVector[i]))
            {
            theSpeedProfile.aMax = static_cast<float>(fabs(theMachineProperties.motors.aMax[i] / theTrajectory.directionUnitVector[i]));			// restrict aMax, if this would result in exceeding aMax for any of the Axis
            }
        if (fabs(theSpeedProfile.dMax) > fabs(theMachineProperties.motors.dMax[i] / theTrajectory.directionUnitVector[i]))
            {
            theSpeedProfile.dMax = static_cast<float>(-1 * fabs(theMachineProperties.motors.dMax[i] / theTrajectory.directionUnitVector[i]));		// restrict dMax, if this would result in exceeding dMax for any of the Axis
            }
        }
    theSpeedProfile.jMax = theMachineProperties.motors.jMax;
    }

void MotionItem::print() const
{
#ifndef WIN32
	Serial.print("MotionType  : ");
	switch (theType)
	{
	case MotionType::Traverse:
		Serial.println("Traverse");
		break;
	case MotionType::FeedLinear:
		Serial.println("FeedLinear");
		break;
	case MotionType::FeedHelicalCW:
		Serial.println("FeedHelicalCW");
		break;
	case MotionType::FeedHelicalCCW:
		Serial.println("FeedHelicalCCW");
		break;
	case MotionType::PauseAndResume:
		Serial.println("PauseAndResume");
		break;
	case MotionType::Pause:
		Serial.println("Pause");
		break;
	default:
		Serial.println("unknown");
		break;
	}

	theTrajectory.print(theType);
	theSpeedProfile.print();
	thePeripherals.print();
#endif
}

void MotionItem::plot(char type, float xMin, float xMax, uint32_t nmbrXSteps, float yMin, float yMax, uint32_t nmbrYSteps) const
{
#ifndef WIN32
	float yStepSize = (yMax - yMin) / nmbrYSteps;
	float xStepSize = (xMax - xMin) / nmbrXSteps;

	Serial.println("");
	switch (type)
	{
	case 'a':
		Serial.println("a(t)");
		break;
	case 'v':
		Serial.println("v(t)");
		break;
	case 's':
	default:
		Serial.println("s(t)");
		break;
	}
	Serial.print("========");
	for (float x = xMin; x <= xMax; x += xStepSize)
	{
		Serial.print("=");
	}
	Serial.println("");

	for (float y = yMax; y >= yMin; y -= yStepSize)
	{
		if (y >= 0.0F)
		{
			Serial.print("+");
		}
		else
		{
			Serial.print("-");
		}
		if (y < 100.0F)
		{
			Serial.print("0");
		}
		if (y < 10.0F)
		{
			Serial.print("0");
		}
		Serial.print(abs(y));
		Serial.print(" ");

		for (float x = xMin; x <= xMax; x += xStepSize)
		{
			float value;
			switch (type)
			{
			case 'a':
				value = a(x);
				break;
			case 'v':
				value = v(x);
				break;
			case 's':
			default:
				value = s(x);
				break;
			}
			if ((value >= y) && (value < y + yStepSize))
			{
				Serial.print("*");
			}
			else
			{
				if (((x <= theSpeedProfile.left.duration) && ((x + xStepSize) > theSpeedProfile.left.duration)) || ((x <= (theSpeedProfile.left.duration + theSpeedProfile.mid.duration)) && ((x + xStepSize) > (theSpeedProfile.left.duration + theSpeedProfile.mid.duration))))
				{
					Serial.print("|");
				}
				else
				{
					Serial.print(" ");
				}
			}
		}
		Serial.println("");
	}
	Serial.print("========");
	for (float x = xMin; x <= xMax; x += xStepSize)
	{
		Serial.print("=");
	}
	Serial.println("");
#endif
}
