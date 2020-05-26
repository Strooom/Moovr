// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "motionctrl.h"

MotionCtrl::MotionCtrl(EventBuffer &theEventbuffer) : theEventBuffer(theEventBuffer), theMachineProperties(theMachineProperties), theOverrides(theOverrides)
    {
//    // Initialize the stepBuffer with some default values
//    stepBufferReadIndex = 0;
//    for (uint32_t i = 0; i < minStepBufferItems; i++)
//        {
//        stepBuffer[i].out = 0x00;
//        stepBuffer[i].reload = defaultReload;
//        stepBufferTotalTime = stepBufferTotalTime + defaultReload;
//        stepBufferLevel++;
//        }
//
//
//
//    // Setup the Periodic interrupt Timer
//    SIM_SCGC6 |= SIM_SCGC6_PIT;											// Activates the clock for PIT0, PIT1 and PIT2
//    PIT_MCR = 0x00;														// PIT Module Control Register (PIT_MCR) This register enables or disables the PIT timer clocks and controls the timers when the PIT enters the Debug mode.
//
//
//    // PIT0 and PIT1 are used for outputting stepper (and peripherals) signals with the correct timing
//    // PIT0 is used as a prescaler to divide the clock by (eg) 750,
//    // PIT1 is chained to PIT0 and used to time the stepper signals
//
//    PIT_LDVAL0 = preScaler - 1;											// reloadValue of X results in a X+1 bus cycle period
////	PIT_LDVAL0 = (preScaler*200) - 1;											// debugging only, slowing it down to 1 reload per second in wait..
//    PIT_TCTRL0 = 0x1;													// Timer Control Register (PIT_TCTRLn) 	Start this timer, but don't do interrupts
//
//    PIT_LDVAL1 = defaultReload - 1;										// Timer Load Value Register (PIT_LDVALn) 	These registers select the timeout period for the timer interrupts.
//    NVIC_ENABLE_IRQ(IRQ_PIT_CH1);										// Enable PIT1 interrupt in the vector-table
//    PIT_TCTRL1 = 0x7;													// Timer Control Register (PIT_TCTRLn). Chain the timer to PIT0, Start the timer and enable interrupts
//
//
//    // PIT2 is used with 5ms periodic interrupts for capturing buttons and limit switches
//    PIT_LDVAL2 = (F_BUS / 200) - 1;										// Timer Load Value Register (PIT_LDVALn)
////	PIT_LDVAL2 = F_BUS - 1;												// Timer Load Value Register (PIT_LDVALn)
//    NVIC_ENABLE_IRQ(IRQ_PIT_CH2);										// Enable PIT2 interrupt in the vector-table
//    PIT_TCTRL2 = 0x3;													// Timer Control Register (PIT_TCTRLn) 	These registers contain the control bits for each timer.start timer and enable interrupts


    // Hardware Pins for Moovr Motherboard
    // OUTPUTS
    // Step and Dir : PC[0..11]
    // Enable for Motors [1..3] : PB18
    // Enable for Motors [4..6] : PB19
    // Peripherals [1..2] : PB[0..1]
    // Peripherals [3..4] : PA[12..13]
    // Peripherals [5] : PA[5]
    // Peripherals [6] : PE[26]


    // Setup the PortC which will output Step and Dir signals
    //PORTC_PCR0 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC0 into Alt 1, High Drive Strength
    //PORTC_PCR1 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC1 into Alt 1, High Drive Strength
    //PORTC_PCR2 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC2 into Alt 1, High Drive Strength
    //PORTC_PCR3 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC3 into Alt 1, High Drive Strength
    //PORTC_PCR4 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC4 into Alt 1, High Drive Strength
    //PORTC_PCR5 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC5 into Alt 1, High Drive Strength
    //PORTC_PCR6 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //PORTC_PCR7 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //PORTC_PCR8 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //PORTC_PCR9 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //PORTC_PCR10 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //PORTC_PCR11 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //GPIOC_PDDR = (0x3F);												// Set PortC[5..0] as Output
    //GPIOC_PDDR = 0x00000FFF;											// Set PortC[0..11] as Output


    // Stepper Driver Enable signals
    //GPIOB_PDDR = 0x000C0000;											// Set PortB[18..19] as Output
    //PORTB_PCR18 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortB[18..19] into Alt 1, High Drive Strength																		// Enable for Motors [1..3] : PB18
    //PORTB_PCR19 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //GPIOB_PDOR = 0;					// Enable Motors 123 and 456 ?
    //GPIOB_PDOR = 0x40000;		// Disable Motors 123 Apparently driving the signal HIGH, Disables the motors, keeping it LOW enables them


    //theOverrides.feedOverride = 1.0F;			// standard feedOverride is 100% of Gcode speed
    //theOverrides.spindleOverride = 1.0F;		// idem for Spindle
    }

void MotionCtrl::append(gCodeParserResult &theParseResult)
    {
    if (!theMotionBuffer.isFull())
        {
		uint32_t newItemIndex = theMotionBuffer.push();																		// add an item to the buffer
		theMotionBuffer.motionBuffer[newItemIndex].set(theParseResult, theMachineProperties, theStrategy, theOverrides);	// populate the motionItem from the results of the gCode parsing
        }
    else
        {
        // Error : motonBuffer overflow..
		// TODO : send a critical error event to the mainController, as this should never happen..
        }
    }

void MotionCtrl::optimize()
    {
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
    //        uint8_t itemLeft;					// index of the left MotionItem (oldest)
    //        uint8_t itemRight;					// index of the right MotionItem (newest)
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

    //                // restrict to maximal vEnd for given vStart of the lefthand motionItem
    //                v = motionBuffer[itemLeft].calcOtherV(MotionStrategy::maximizeSpeed, true);
    //                if (v < vJunction)
    //                    {
    //                    vJunction = v;
    //                    }

    //                // restrict to maximal vStart for given vEnd of the righthand motionItem
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

step MotionCtrl::getNextStep()
    {
	// temp code : needs to return something to make the compiler happy
	return step {0,0};

    //while (true)
    //    {
    //    timeInMotionTicks++;																// Determine sampling time as 'previous sample time' + sampling interval (1)
    //    lastReload++;
    //    while (true)
    //        {
    //        if (motionBufferLevel > 0)
    //            {
    //            MotionItem* currentMotionItem = &motionBuffer[motionBufferReadIndex];		// pointer to active motion, speeds up accessing its members
    //            float timeInMotionFloat;													// equivalent in floating point seconds
    //            timeInMotionFloat = (float)timeInMotionTicks * timerTick;					// convert discrete time in timerticks to float time in seconds

    //            if (timeInMotionFloat > currentMotionItem->tMotion)
    //                {
    //                timeInMotionTicks = timeInMotionTicks - (uint32_t)((currentMotionItem->tMotion) * timerFrequency);
    //                //timeInMotionFloat = timeInMotionFloat - currentMotionItem->tMotion;

    //                motionBufferReadIndex = (motionBufferReadIndex + 1) % motionBufferLength;
    //                --motionBufferLevel;

    //                //pushEvent(Event::limitSwitchXMaxOpened);	//fire a motionComplete event();
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

void MotionCtrl::fill(step anItem)
    {
    //if (stepBufferLevel < stepBufferLength)																// only if buffer not full
    //    {
    //    uint16_t stepBufferWriteIndex;
    //    //disable_interrups();		// lock
    //    stepBufferTotalTime = stepBufferTotalTime + anItem.reload;										// adjust total time in buffer
    //    if (stepBufferLevel > 2)																		// This is the normal case, as we try to keep the stepBuffer filled with enough steps
    //        {
    //        stepBufferWriteIndex = (stepBufferReadIndex + stepBufferLevel - 2) % stepBufferLength;		// Calculate writeIndex for TimerReload as offset from readIndex. This is actually 2 positions back in the buffer
    //        stepBuffer[stepBufferWriteIndex].reload = anItem.reload;									// write the TimerReload
    //        stepBufferWriteIndex = (stepBufferWriteIndex + 2) % stepBufferLength;						// Calculate writeIndex for Outputs
    //        stepBuffer[stepBufferWriteIndex].out = anItem.out;											// write the Output bits
    //        stepBufferLevel++;																			// adjust the bufferLevel
    //        }
    //    else																							// This is the startup case, buffer is empty but we have motions to fill it
    //        {
    //        // this should never happen... but if it does how do we handle it
    //        }
    //    //enable_interrups();		// unlock
    //    }
    }

void MotionCtrl::output()
    {
//    if (stepBufferLevel >= minStepBufferItems)
//        {
//#ifndef WIN32
//        PIT_LDVAL1 = stepBuffer[stepBufferReadIndex].reload;									// reload timer
//        GPIOC_PDOR = stepBuffer[stepBufferReadIndex].out;
//
//        //logger.log("output:");
//        //logger.log(stepBuffer[stepBufferReadIndex].out);
//        //logger.logNow("\n");
//
//#endif
////        stepBufferTotalTime = stepBufferTotalTime - stepBuffer[stepBufferReadIndex].reload;		// adjust total time in buffer
//        stepBufferReadIndex = (stepBufferReadIndex + 1) % stepBufferLength;						// adjust read pointer to next position
//        stepBufferLevel--;																		// adjust stepBufferLevel to one less than before
//        }
//    else
//        {
//        // buffer underrun... should not happen
//        }
    }

void MotionCtrl::run()
    {
//    // 1. Fill the buffer when needed
//    while ((stepBufferLevel < minStepBufferItems) || ((stepBufferTotalTime < minStepBufferTotalTime) && (stepBufferLevel < stepBufferLength)))
//        {
//        stepperItem anItem = getNextStep();				// get next step from Motion...
//        fill(anItem);									// ... and pump it to stepBuffer
//        }
//
//#ifdef WIN32
//    // 2. Only in Windows, output from the buffer instead of bufferRead being triggered from timer interrupt
//    while (stepBufferLevel >= minStepBufferItems)
//        {
//        output();
//        }
//#endif
    }

void MotionCtrl::toString()
    {
    //for (uint32_t i = 0; i < motionBufferLevel; i++)
    //    {
    //    motionBuffer[(motionBufferReadIndex + i) % motionBufferLength].toString();
    //    }


    //for (uint16_t i = 0; i < stepBufferLength; i++)
    //    {
    //    snprintf(logger.logLine, 127, "%x %d \n", stepBuffer[i].out, stepBuffer[i].reload);
    //    logger.logNow();
    //    }
    }

