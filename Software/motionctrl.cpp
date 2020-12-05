// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "motionctrl.h"
#include "logging.h"
#include <math.h>


extern uLog theLog;

motionCtrl::motionCtrl(eventBuffer &theEventBuffer, machineProperties &theMachineProperties, overrides &theOverrides, stepBuffer &theStepBuffer) : 
    theEventBuffer(theEventBuffer), theMachineProperties(theMachineProperties), theOverrides(theOverrides), theStepBuffer(theStepBuffer) {
}

void motionCtrl::append(gCodeParserResult &theParseResult) {
    if (!theMotionBuffer.isFull()) {
        uint32_t newItemIndex = theMotionBuffer.push();                                                                         // add an item to the buffer
        theMotionBuffer.motionBuffer[newItemIndex].set(theParseResult, theMachineProperties, theStrategy, theOverrides);        // populate the motion from the results of the gCode parsing
    } else {
        // Error : motonBuffer overflow..
        // TODO : send a critical error event to the mainController, as this should never happen..
    }
}

void motionCtrl::optimize() {
    //switch (motionBufferLevel)
    //    {
    //    case 0:									// empty motionBuffer - nothing to do as there is nothing to optimize
    //        break;

    //    case 1:									// only 1 item, which is currently being executed -> derive a new motion for the remaining part, and optimize it's speed
    //        motionBuffer[motionBufferReadIndex].adjustRemaining();			// TODO : create new item from remaining part of motion...
    //        motionBuffer[motionBufferReadIndex].optimize(theStrategy, theOverrides, theMachineProperties);
    //        break;

    //    default:								// 2 or more items -> optimize them in pairs, each call of this function optimizes 1 pair.
    //        // minimizing is done by taking the pairs from left to right = oldest to newest.
    //        // maximizing is done by taking the pairs from right to left = newest to oldest
    //        // As only the junction speed of a pair is modified (not the entry/exit speed of the pair) it takes several iterations over the complete buffer to arrive a full optimization
    //        uint8_t itemLeft;					// index of the left motion (oldest)
    //        uint8_t itemRight;					// index of the right motion (newest)
    //        float vJunction;					// exit-entry speed between the motion-pair after optimizing
    //        float v;							// local variable to calculate and test for speed conditions

    //        switch (theStrategy)
    //            {
    //            case MotionStrategy::minimizeSpeed:
    //                // minimizing speed -> iterate from left to right over motions
    //                if (junctionIndex > motionBufferLevel - 2)
    //                    {
    //                    junctionIndex = 0;
    //                    }
    //                else
    //                    {
    //                    junctionIndex++;
    //                    }
    //                break;

    //            case MotionStrategy::maximizeSpeed:
    //                // maximize speed -> iterate from right to left over motions
    //                if (0 == junctionIndex)
    //                    {
    //                    junctionIndex = motionBufferLevel - 2;
    //                    }
    //                else
    //                    {
    //                    junctionIndex--;
    //                    }
    //                break;
    //            }

    //        itemLeft = (motionBufferReadIndex + junctionIndex) % motionBufferLength;
    //        itemRight = (motionBufferReadIndex + junctionIndex + 1) % motionBufferLength;

    //        if ((itemLeft == motionBufferReadIndex) || (itemRight == motionBufferReadIndex))
    //            {
    //            motionBuffer[motionBufferReadIndex].adjustRemaining();			// create new item from remaining part of motion...
    //            }

    //        switch (theStrategy)
    //            {
    //            case MotionStrategy::minimizeSpeed:
    //                vJunction = 0; // assume we can slow down to zero, adjust upwards if needed by other constraints

    //                v = motionBuffer[itemLeft].calcOtherV(MotionStrategy::minimizeSpeed, true);
    //                if (v > vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                v = motionBuffer[itemRight].calcOtherV(MotionStrategy::minimizeSpeed, false);
    //                if (v > vJunction)
    //                    {
    //                    vJunction = v;
    //                    }
    //                break;

    //            case MotionStrategy::maximizeSpeed:
    //                // calculate the maximal vEnd/vStart = vJunction, starting from a very large value, then restricting it down so it meets all kind of constraints
    //                vJunction = 999999999; // some large value to be restricted down... TODO : makes this cleaner eg using HUGE_VALF

    //                // restrict to maximal vEnd for given vStart of the lefthand motion
    //                v = motionBuffer[itemLeft].calcOtherV(MotionStrategy::maximizeSpeed, true);
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                // restrict to maximal vStart for given vEnd of the righthand motion
    //                v = motionBuffer[itemRight].calcOtherV(MotionStrategy::maximizeSpeed, false);
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                // restrict to respect feedrates and machine limits of both left and right
    //                v = motionBuffer[itemLeft].vFeed * theOverrides.feedOverride;
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                v = motionBuffer[itemLeft].vMax;
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                v = motionBuffer[itemRight].vFeed * theOverrides.feedOverride;
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                v = motionBuffer[itemRight].vMax;
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                // TODO : vJunction needs to be further reduced according to the kind of corners the trajectory is taking...
    //                break;
    //            }

    //        // now with this optimal junction speed, recalculate both motionItems
    //        motionBuffer[itemLeft].vEnd = vJunction;
    //        motionBuffer[itemRight].vStart = vJunction;
    //        motionBuffer[itemLeft].optimize(theStrategy, theOverrides, theMachineProperties);
    //        motionBuffer[itemRight].optimize(theStrategy, theOverrides, theMachineProperties);
    //    }
}

step motionCtrl::getNextStep() {
    // temp code : needs to return something to make the compiler happy
    return step{0, 0};

    //while (true)
    //    {
    //    timeInMotionTicks++;																// Determine sampling time as 'previous sample time' + sampling interval (1)
    //    lastReload++;
    //    while (true)
    //        {
    //        if (motionBufferLevel > 0)
    //            {
    //            motion* currentMotionItem = &motionBuffer[motionBufferReadIndex];		// pointer to active motion, speeds up accessing its members
    //            float timeInMotionFloat;													// equivalent in floating point seconds
    //            timeInMotionFloat = (float)timeInMotionTicks * timerTick;					// convert discrete time in timerticks to float time in seconds

    //            if (timeInMotionFloat > currentMotionItem->tMotion)
    //                {
    //                timeInMotionTicks = timeInMotionTicks - (uint32_t)((currentMotionItem->tMotion) * timerFrequency);
    //                //timeInMotionFloat = timeInMotionFloat - currentMotionItem->tMotion;

    //                motionBufferReadIndex = (motionBufferReadIndex + 1) % motionBufferLength;
    //                --motionBufferLevel;

    //                //pushEvent(event::limitSwitchXMaxOpened);	//fire a motionComplete event();
    //                continue;
    //                }
    //            else
    //                {
    //                float sNow = currentMotionItem->s(timeInMotionFloat);

    //                uint32_t mask = 0x00000001;	// initialize bitmask needed for manipulating Step and Dir bits
    //                // 4. For each axis..
    //                for (uint8_t i = 0; i < (uint8_t)axis::nmbrAxis; ++i)
    //                    {
    //                    // 4a. Determine the position for this axis for this distance travelled
    //                    if (currentMotionItem->deltaRealTime[i] != 0.0F)			// Optimization : only do calculations for Axis that are moving
    //                        {
    //                        if (i == (uint8_t)currentMotionItem->arcAxis0)
    //                            {
    //                            nextPositionInMm[i] = (currentMotionItem->arcCenter0 + (currentMotionItem->radius * cosf(currentMotionItem->startAngle + (currentMotionItem->deltaRealTime[i] * sNow))));
    //                            }
    //                        else if (i == (uint8_t)currentMotionItem->arcAxis1)
    //                            {
    //                            nextPositionInMm[i] = (currentMotionItem->arcCenter1 + (currentMotionItem->radius * sinf(currentMotionItem->startAngle + (currentMotionItem->deltaRealTime[i] * sNow))));
    //                            }
    //                        else
    //                            {
    //                            nextPositionInMm[i] = (currentMotionItem->startPosition[i] + currentMotionItem->deltaRealTime[i] * sNow);
    //                            }

    //                        // 4b. Determine if this new position requires a step, forward or backwards
    //                        // NOTE - TODO there is some problem here as the conversion from float to int rounds towards zero (discards the fractional part) which means the interval [-1,1] takes twice the time..
    //                        // Should test this on the K64/K66 to see how it behaves there
    //                        if ((int32_t)((nextPositionInMm[i] * theMachineProperties.motors.stepsPerMm[i]) - hysteresis) > currentPositionInSteps[i])         // Determine if this position involves step forward...
    //                            {
    //                            // Step forward
    //                            //logger.logNow("Step++\n");
    //                            dirSetup = dirSetup & ~mask;			// mask step signal to zero (should already be zero)
    //                            stepRise = stepRise | mask;				// set step pulse high
    //                            stepFall = stepFall & ~mask;			// return step pulse low
    //                            mask = mask << 1;						// adjust mask for direction bits
    //                            dirSetup = dirSetup | mask;				// set direction high
    //                            stepRise = stepRise | mask;				// set direction high
    //                            stepFall = stepFall | mask;				// set direction high
    //                            mask = mask << 1;						// adjust mask for next iteration = next axis
    //                            currentPositionInSteps[i]++;
    //                            }
    //                        else if ((int32_t)((nextPositionInMm[i] * theMachineProperties.motors.stepsPerMm[i]) + hysteresis) < currentPositionInSteps[i])		// ... or a step backwards
    //                            {
    //                            // Step backwards
    //                            dirSetup = dirSetup & ~mask;			// mask step signal to zero (should already be zero)
    //                            stepRise = stepRise | mask;				// set step pulse high
    //                            stepFall = stepFall & ~mask;			// return step pulse low
    //                            mask = mask << 1;						// adjust mask for direction bits
    //                            dirSetup = dirSetup & ~mask;			// set direction low
    //                            stepRise = stepRise & ~mask;			// set direction low
    //                            stepFall = stepFall & ~mask;			// set direction low
    //                            mask = mask << 1;						// adjust mask for next iteration = next axis
    //                            currentPositionInSteps[i]--;
    //                            }
    //                        else
    //                            {
    //                            mask = mask << 2;
    //                            }
    //                        }
    //                    else
    //                        {
    //                        mask = mask << 2;
    //                        }
    //                    }
    //                if ((dirSetup != lastOut) || (lastReload >= defaultReload))
    //                    {
    //                    lastOut = dirSetup;
    //                    stepperItem output;
    //                    output.out = dirSetup;
    //                    output.reload = lastReload-1;
    //                    lastReload = 0;

    //                    // Shift the bitpatterns for the next run..
    //                    dirSetup = stepRise;
    //                    stepRise = stepFall;
    //                    return output;
    //                    }
    //                }
    //            }
    //        else
    //            {
    //            // no more motionItems in the buffer -> repeat whatever is already written out
    //            timeInMotionTicks = 0;
    //            stepperItem output;
    //            output.out = lastOut;
    //            output.reload = defaultReload-1;
    //            return output;
    //            }
    //        break;
    //        }
    //    // shift the bitpatterns for the next run..
    //    dirSetup = stepRise;
    //    stepRise = stepFall;
    //    }
}

void motionCtrl::run() {
    while (theStepBuffer.needsFilling()) {
        step aStep = getNextStep();        // get next step from Motion...
        theStepBuffer.write(aStep);        // ... and pump it to buffer
    }
}
