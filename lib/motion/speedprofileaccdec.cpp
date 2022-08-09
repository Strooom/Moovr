// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

// TODO : should I make this function more robust by handling invalid inputs, eg aMax/dMax/jMax = 0 or negative vStart/vEnd
// TODO : maybe it is enough to keep one adMax per part, rather than separate aMax and dMax

#include "speedprofileaccdec.h"
#include <stdio.h>        // needed for sprintf()
#include "math.h"         // needed for sqrtf()

speedProfileAccDec::speedProfileAccDec() : vStart{0.0F}, vEnd{0.0F}, aMax{0.0F}, dMax{0.0F}, jMax{0.0F}, dV1{0.0F}, s1{0.0F}, s2{0.0F}, s3{0.0F}, length{0.0F}, d1d3{0.0F}, d2{0.0F}, duration{0.0F} {
}

speedProfileAccDec::speedProfileAccDec(float vStart, float vEnd, float aMax, float dMax, float jMax) : vStart(vStart), vEnd(vEnd), aMax(aMax), dMax(dMax), jMax(jMax),        // initialize inputs
                                                                                                       dV1(0.0F),
                                                                                                       s1(0.0F),
                                                                                                       s2(0.0F),
                                                                                                       s3(0.0F),
                                                                                                       length(0.0F),        // clear outputs
                                                                                                       d1d3(0.0F),
                                                                                                       d2(0.0F),
                                                                                                       duration(0.0F)        // clear outputs
                                                                                                       {
                                                                                                           // TODO : how to deal with aMax or dMax == 0 ?? How should we escalate this invalid input ?? It can lead to sqrt() failing further down the road
                                                                                                           // TODO : how to deal with negative inputs for vStart and vMax ??
                                                                                                           // TODO : does it make sense to have vStart == vEnd ??
                                                                                                       };

void speedProfileAccDec::setMax(float a, float d, float j) {
    aMax = a;
    dMax = d;
    jMax = j;
}

void speedProfileAccDec::setvStart(float v) {
    vStart = v;
}

void speedProfileAccDec::setvEnd(float v) {
    vEnd = v;
}

void speedProfileAccDec::calculate(speedProfileOrder theOrder) {
    switch (theOrder) {
        default:
        case speedProfileOrder::firstOrder:
            // all properties for the second order are set to zero
            d1d3 = 0.0F;
            s1   = 0.0F;
            s3   = 0.0F;
            dV1  = 0.0F;

            if (vStart < vEnd) {
                d2 = (vEnd - vStart) / aMax;
            } else {
                d2 = (vEnd - vStart) / dMax;
            }
            s2       = (vStart + vEnd) * d2 * 0.5F;
            length   = s2;
            duration = d2;
            break;

        case speedProfileOrder::secondOrder:
            float dV2;
            if (vStart < vEnd) {
                // BUG : aMax here should be the motion aMax and not the speedProfile, as this may have been recalculated down due to overrides etc.. we always need to restart from a fresh aMax
                d1d3 = aMax / jMax;                                // Maximal duration of phase 1 and 3
                if ((jMax * d1d3 * d1d3) > (vEnd - vStart))        // Do we need a phase 2 ?
                {
                    d1d3 = sqrtf((vEnd - vStart) / jMax);        // No, so adjust d1d3...
                    aMax = jMax * d1d3;
                    d2   = 0.0F;        // and no need for phase 2
                    // dV2  = 0.0F;        // TODO : why is this needed ? dV2 is a local var not referenced further below ??
                } else {
                    dV2 = (vEnd - vStart) - (jMax * d1d3 * d1d3);        // Yes, phase 1 and 3 are ok, but we need to add a phase 2
                    d2  = dV2 / aMax;
                }
                dV1 = jMax * d1d3 * d1d3 * 0.5F;
                s1  = (vStart * d1d3) + jMax * d1d3 * d1d3 * d1d3 * oneSixth;
                s2  = (vStart + vEnd) * d2 * 0.5F;
                s3  = (vEnd * d1d3) - jMax * d1d3 * d1d3 * d1d3 * oneSixth;
            } else {
                d1d3 = -dMax / jMax;                               // Maximal duration of phase 1 and 3
                if ((jMax * d1d3 * d1d3) > (vStart - vEnd))        // Do we need a phase 2 ?
                {
                    d1d3 = sqrtf((vStart - vEnd) / jMax);        // No, so adjust d1d3...
                    dMax = -jMax * d1d3;
                    d2   = 0.0F;        // and no need for phase 2
                    // dV2  = 0.0F;        // TODO : why is this needed ? dV2 is a local var not referenced further below ??
                } else {
                    dV2 = (vEnd - vStart) + (jMax * d1d3 * d1d3);        // Yes, phase 1 and 3 are ok, but we need to add a phase 2
                    d2  = dV2 / dMax;
                }
                dV1 = -jMax * d1d3 * d1d3 * 0.5F;
                s1  = (vStart * d1d3) - jMax * d1d3 * d1d3 * d1d3 * oneSixth;
                s2  = (vStart + vEnd) * d2 * 0.5F;
                s3  = (vEnd * d1d3) + jMax * d1d3 * d1d3 * d1d3 * oneSixth;
            }
            length   = (vStart + vEnd) * (d1d3 + d2 + d1d3) * 0.5F;
            duration = d1d3 + d2 + d1d3;
            break;
    }
}

float speedProfileAccDec::calcSpeed() const        // Calculate the (exit) speed of a MotionPart given vStart, vEnd (just to indicate acceleration or deceleration) and length
{
    float ad;
    ad = (vStart < vEnd) ? aMax : dMax;
    return sqrtf(vStart * vStart + 2 * ad * length);
    // TODO : I think this might be redundant/overlap with vOut() function..
}

float speedProfileAccDec::a(float t) const {
    if (vEnd > vStart) {
        // accelerating
        if (t < d1d3) {
            return (jMax * t);
        } else if (t > d1d3 + d2) {
            float tDelta = ((d1d3 + d2 + d1d3) - t);
            return (jMax * tDelta);
        } else {
            return aMax;
        }
    } else {
        // decelerating
        if (t < d1d3) {
            return -1.0F * (jMax * t);
        } else if (t > d1d3 + d2) {
            float tDelta = ((d1d3 + d2 + d1d3) - t);
            return -1.0F * (jMax * tDelta);
        } else {
            return dMax;
        }
    }
}

float speedProfileAccDec::v(float t) const {
    if (vEnd > vStart) {
        // accelerating
        if (t < d1d3) {
            return vStart + (jMax * t * t * 0.5F);
        } else if (t > d1d3 + d2) {
            float tDelta = (duration - t);
            return vEnd - (jMax * tDelta * tDelta * 0.5F);
        } else {
            float tDelta = (t - d1d3);
            return vStart + dV1 + (aMax * tDelta);
        }
    } else {
        // decelerating
        if (t < d1d3) {
            return vStart - (jMax * t * t * 0.5F);
        } else if (t > d1d3 + d2) {
            float tDelta = (duration - t);
            return vEnd + (jMax * tDelta * tDelta * 0.5F);
        } else {
            float tDelta = (t - d1d3);
            return vStart + dV1 + (dMax * tDelta);
        }
    }
}

float speedProfileAccDec::s(float t) const {
    if (vEnd > vStart) {
        // accelerating
        if (t < d1d3) {
            return (vStart * t) + (jMax * t * t * t * oneSixth);
        } else if (t > d1d3 + d2) {
            float tDelta = (duration - t);
            return length - ((vEnd * tDelta) - (jMax * tDelta * tDelta * tDelta * oneSixth));
        } else {
            float tDelta = (t - d1d3);
            return s1 + ((vStart + dV1) * tDelta) + (aMax * tDelta * tDelta * 0.5F);
        }
    } else {
        // decelerating
        if (t < d1d3) {
            return (vStart * t) - (jMax * t * t * t * oneSixth);
        } else if (t > d1d3 + d2) {
            float tDelta = (duration - t);
            return length - ((vEnd * tDelta) + (jMax * tDelta * tDelta * tDelta * oneSixth));
        } else {
            float tDelta = (t - d1d3);
            return s1 + ((vStart + dV1) * tDelta) + (dMax * tDelta * tDelta * 0.5F);
        }
    }
}

uint32_t speedProfileAccDec::toString(char* output) const {
    uint32_t outputLenght{0};

    outputLenght += sprintf(output + outputLenght, "vStart   = %f mm/s\n", vStart);
    outputLenght += sprintf(output + outputLenght, "vEnd     = %f mm/s\n", vEnd);
    outputLenght += sprintf(output + outputLenght, "aMax     = %f mm/s^2\n", aMax);
    outputLenght += sprintf(output + outputLenght, "dMax     = %f mm/s^2\n", dMax);
    outputLenght += sprintf(output + outputLenght, "jMax     = %f mm/s^3\n", jMax);
    outputLenght += sprintf(output + outputLenght, "dV1      = %f mm/s\n", dV1);
    outputLenght += sprintf(output + outputLenght, "s1       = %f mm\n", s1);
    outputLenght += sprintf(output + outputLenght, "s2       = %f mm\n", s2);
    outputLenght += sprintf(output + outputLenght, "s3       = %f mm\n", s3);
    outputLenght += sprintf(output + outputLenght, "length   = %f mm\n", length);
    outputLenght += sprintf(output + outputLenght, "d1d3     = %f s\n", d1d3);
    outputLenght += sprintf(output + outputLenght, "d2       = %f s\n", d2);
    outputLenght += sprintf(output + outputLenght, "duration = %f s\n", duration);

    return outputLenght;
}
