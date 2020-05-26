// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

// TODO : should I make this function more robust by handling invalid inputs, eg aMax/dMax/jMax = 0 or negative vStart/vEnd
// TODO : maybe it is enough to keep one adMax per part, rather than separate aMax and dMax

#include "speedprofile.h"

// A motionpart is consist of the acceleration OR deceleration part of a motion.
// The mid / cruise part of a motion is not part of the motionpart
// As such, a motionitem consists of a (left) motionpart, a mid/cruise and a (right) motionpart

SpeedProfileAccDec::SpeedProfileAccDec() : vStart{ 0.0F }, vEnd{ 0.0F }, aMax{ 0.0F }, dMax{ 0.0F }, jMax{ 0.0F }, dV1{ 0.0F }, s1{ 0.0F }, s2{ 0.0F }, s3{ 0.0F }, length{ 0.0F }, d1d3{ 0.0F }, d2{ 0.0F }, duration{ 0.0F }
    {
    }

//SpeedProfileAccDec::SpeedProfileAccDec(float vStart, float vEnd, float aMax, float dMax, float jMax) :
//    vStart(vStart), vEnd(vEnd), aMax(aMax), dMax(dMax), jMax(jMax),														// initialize inputs
//    dV1(0.0F), s1(0.0F), s2(0.0F), s3(0.0F), length(0.0F),																			// clear outputs
//    d1d3(0.0F), d2(0.0F), duration(0.0F)																								// clear outputs
//    {
//    // TODO : how to deal with aMax or dMax == 0 ?? How should we escalate this invalid input ?? It can lead to sqrt() failing further down the road
//    // TODO : how to deal with negative inputs for vStart and vMax ??
//    // TODO : does it make sense to have vStart == vEnd ??
//    };

void SpeedProfileAccDec::setMax(float a, float d, float j)
    {
    aMax = a;
    dMax = d;
    jMax = j;
    }

void SpeedProfileAccDec::setvStart(float v)
    {
    vStart = v;
    }

void SpeedProfileAccDec::setvEnd(float v)
    {
    vEnd = v;
    }

void SpeedProfileAccDec::calculate(MotionSpeedProfileOrder theProfile)
    {
    switch (theProfile)
        {
        default:
        case MotionSpeedProfileOrder::firstOrder:
			// all properties for the second order are set to zero
			d1d3 = 0.0F;
			s1 = 0.0F;
			s3 = 0.0F;
			dV1 = 0.0F;

            if (vStart < vEnd)
                {
                d2 = (vEnd - vStart) / aMax;
                }
            else
                {
                d2 = (vEnd - vStart) / dMax;
                }
            s2 = (vStart + vEnd) * d2 * 0.5f;
			length = s2;
            duration = d2;
            break;

        case MotionSpeedProfileOrder::secondOrder:
            float dV2;
            if (vStart < vEnd)
                {
                d1d3 = aMax / jMax;								// Maximal duration of phase 1 and 3
                if ((jMax * d1d3 * d1d3) > (vEnd - vStart))		// Do we need a phase 2 ?
                    {
                    d1d3 = sqrtf((vEnd - vStart) / jMax);		// No, so adjust d1d3...
                    aMax = jMax * d1d3;
                    d2 = 0.0F;									// and no need for phase 2
                    dV2 = 0.0F;
                    }
                else
                    {
                    dV2 = (vEnd - vStart) - (jMax * d1d3 * d1d3);	// Yes, phase 1 and 3 are ok, but we need to add a phase 2
                    d2 = dV2 / aMax;
                    }
                dV1 = jMax * d1d3 * d1d3 * 0.5f;
                s1 = (vStart * d1d3) + jMax * d1d3 * d1d3 * d1d3 * oneSixth;
                s2 = (vStart + vEnd) * d2 * 0.5f;
                s3 = (vEnd * d1d3) - jMax * d1d3 * d1d3 * d1d3 * oneSixth;
                }
            else
                {
                d1d3 = -dMax / jMax;							// Maximal duration of phase 1 and 3
                if ((jMax * d1d3 * d1d3) > (vStart - vEnd))		// Do we need a phase 2 ?
                    {
                    d1d3 = sqrtf((vStart - vEnd) / jMax);		// No, so adjust d1d3...
                    dMax = -jMax * d1d3;
                    d2 = 0.0F;									// and no need for phase 2
                    dV2 = 0.0F;
                    }
                else
                    {
                    dV2 = (vEnd - vStart) + (jMax * d1d3 * d1d3);	// Yes, phase 1 and 3 are ok, but we need to add a phase 2
                    d2 = dV2 / dMax;
                    }
                dV1 = -jMax * d1d3 * d1d3 * 0.5f;
                s1 = (vStart * d1d3) - jMax * d1d3 * d1d3 * d1d3 * oneSixth;
                s2 = (vStart + vEnd) * d2 * 0.5f;
                s3 = (vEnd * d1d3) + jMax * d1d3 * d1d3 * d1d3 * oneSixth;
                }
            break;
        }
    length = (vStart + vEnd) * (d1d3 + d2 + d1d3) * 0.5f;
    duration = d1d3 + d2 + d1d3;
    }

float SpeedProfileAccDec::calcSpeed() const							// Calculate the (exit) speed of a MotionPart given vStart, vEnd (just to indicate acceleration or deceleration) and length
    {
    float ad;
    ad = (vStart < vEnd) ? aMax : dMax;
    return sqrtf(vStart * vStart + 2 * ad * length);
    }

float SpeedProfileAccDec::a(float t) const
    {
    if (vEnd > vStart)
        {
        // accelerating
        if (t <= d1d3)
            {
            return  (jMax * t);
            }
        else if (t <= d1d3 + d2)
            {
            return aMax;
            }
        else
            {
            float tDelta = ((d1d3 + d2 + d1d3) - t);
            return (jMax * tDelta);
            }
        }
    else
        {
        // decelerating
        if (t <= d1d3)
            {
            return  -1.0F * (jMax * t);
            }
        else if (t <= d1d3 + d2)
            {
            return dMax;
            }
        else
            {
            float tDelta = ((d1d3 + d2 + d1d3) - t);
            return -1.0F * (jMax * tDelta);
            }
        }
    }

float SpeedProfileAccDec::v(float t) const
    {
    if (vEnd > vStart)
        {
        // accelerating
        if (t <= d1d3)
            {
            return vStart + (jMax * t * t * 0.5f);
            }
        else if (t <= d1d3 + d2)
            {
            float tDelta = (t - d1d3);
            return vStart + dV1 + (aMax * tDelta);
            }
        else
            {
            float tDelta = (duration - t);
            return vEnd - (jMax * tDelta * tDelta * 0.5f);
            }
        }
    else
        {
        // decelerating
        if (t <= d1d3)
            {
            return vStart - (jMax * t * t * 0.5f);
            }
        else if (t <= d1d3 + d2)
            {
            float tDelta = (t - d1d3);
            return vStart + dV1 + (dMax * tDelta);
            }
        else
            {
            float tDelta = (duration - t);
            return vEnd + (jMax * tDelta * tDelta * 0.5f);
            }
        }
    }

float SpeedProfileAccDec::s(float t) const
    {
    if (vEnd > vStart)
        {
        // accelerating
        if (t <= d1d3)
            {
            return (vStart * t) + (jMax * t * t * t * oneSixth);
            }
        else if (t <= d1d3 + d2)
            {
            float tDelta = (t - d1d3);
            return s1 + ((vStart + dV1) * tDelta) + (aMax * tDelta * tDelta * 0.5f);
            }
        else
            {
            float tDelta = (duration - t);
            return length - ((vEnd * tDelta) - (jMax * tDelta * tDelta * tDelta * oneSixth));
            }
        }
    else
        {
        // decelerating
        if (t <= d1d3)
            {
            return (vStart * t) - (jMax * t * t * t * oneSixth);
            }
        else if (t <= d1d3 + d2)
            {
            float tDelta = (t - d1d3);
            return s1 + ((vStart + dV1) * tDelta) + (dMax * tDelta * tDelta * 0.5f);
            }
        else
            {
            float tDelta = (duration - t);
            return length - ((vEnd * tDelta) + (jMax * tDelta * tDelta * tDelta * oneSixth));
            }
        }
    }

void SpeedProfileAccDec::print() const
    {
#ifndef  WIN32
    Serial.println("Inputs : ");

    Serial.print(" vStart =   ");
    Serial.print(vStart);
    Serial.println(" mm/s");

    Serial.print(" vEnd =     ");
    Serial.print(vEnd);
    Serial.println(" mm/s");

    Serial.print(" aMax =     ");
    Serial.print(aMax);
    Serial.println(" mm/s^2");

    Serial.print(" dMax =     ");
    Serial.print(dMax);
    Serial.println(" mm/s^2");

    Serial.print(" jMax =     ");
    Serial.print(jMax);
    Serial.println(" mm/s^3");

    Serial.println("Outputs Speed : ");
    Serial.print(" dV1 =      ");
    Serial.print(dV1);
    Serial.println(" mm/s");


    Serial.println("Outputs Distance : ");
    Serial.print(" s1 =       ");
    Serial.print(s1);
    Serial.println(" mm");
    Serial.print(" s2 =       ");
    Serial.print(s2);
    Serial.println(" mm");
    Serial.print(" s3 =       ");
    Serial.print(s3);
    Serial.println(" mm");
    Serial.print(" length =   ");
    Serial.print(length);
    Serial.println(" mm");

    Serial.println("Outputs Timing : ");
    Serial.print(" d1 =       ");
    Serial.print(d1d3);
    Serial.println(" s");
    Serial.print(" d2 =       ");
    Serial.print(d2);
    Serial.println(" s");
    Serial.print(" d3 =       ");
    Serial.print(d1d3);
    Serial.println(" s");
    Serial.print(" duration = ");
    Serial.print(duration);
    Serial.println(" s");
#endif
    }

void SpeedProfileAccDec::plot(char type, float xMin, float xMax, uint32_t nmbrXSteps, float yMin, float yMax, uint32_t nmbrYSteps) const
    {
#ifndef  WIN32

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
                if (((x <= d1d3) && ((x + xStepSize) > d1d3)) || ((x <= (d1d3 + d2)) && ((x + xStepSize) > (d1d3 + d2))))
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

SpeedProfileCruise::SpeedProfileCruise() : vMid{ 0.0F }, length{ 0.0F }, duration{ 0.0F }
    {
    }

void SpeedProfileCruise::set(float v)
    {
    vMid = v;
    }

void SpeedProfileCruise::print() const
    {
    // TODO : print properties
    }

float SpeedProfileCruise::a(float t) const
    {
    return 0.0F;
    }

float SpeedProfileCruise::v(float t) const
    {
    return vMid;
    }

float SpeedProfileCruise::s(float t) const
    {
    return vMid * t;
    }
