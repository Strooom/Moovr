// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include <math.h>
#include "logging.h"
#include "maincontroller.h"
#include "gcode.h"
#include "motion.h"

#ifdef log_all
extern Logger logger;
#define log_motion_class				// logging output for the motion class/object
#define log_stepper_class
//#define log_motion_buffer
//#define log_motion_buffer3
#endif

// #####################
// ### MotionPart    ###
// #####################

// A motionpart is consist of the acceleration or deceleration part of a motion.
// The mid / cruise part of a motion is not part of the motionpart
// As such, a motionitem consists of a (left) motionpart, a mid/cruise and a (right) motionpart

MotionPart::MotionPart(MotionSpeedProfile theProfile, float vStart, float vEnd, float aMax, float dMax, float jMax) : theProfile(theProfile), vStart(vStart), vEnd(vEnd), aMax(aMax), dMax(dMax), jMax(jMax)
    {
    };

float MotionPart::calcLength() const						// Calculate the length of a section, given speed and timing parameters
    {
    return (vStart + vEnd) * (d1d3 + d2 + d1d3) * 0.5f;
    }

float MotionPart::calcSpeed() const							// Calculate the (exit) speed of a MotionPart given vStart, vEnd (just to indicate acceleration or deceleration) and length
    {
    float ad;
    ad = (vStart < vEnd) ? aMax : dMax;
    return sqrtf(vStart * vStart + 2 * ad * length);
    }

void MotionPart::calculate()
    {
    switch (theProfile)
        {
        case MotionSpeedProfile::zeroOrder:
            // this doesn't make much sense, as in a zero-order speed-profile, the acceleration and deceleration phase are empty (infinite acceleration/deceleration in zero time)
            // still, to make this function work for all types of speedprofiles, we simply set all values to zero..
            s1 = s2 = s3 = 0.0f;
            d1d3 = d2 = 0.0f;
            length = 0.0f;
            break;

        case MotionSpeedProfile::firstOrder:
            d1d3 = 0.0f;
            s1 = 0.0f;
            s3 = 0.0f;
            if (vStart < vEnd)
                {
                d2 = (vEnd - vStart) / aMax;
                }
            else
                {
                d2 = (vEnd - vStart) / dMax;
                }
            s2 = (vStart + vEnd) * d2 * 0.5f;
            break;

        case MotionSpeedProfile::secondOrder:
            float dV2;
            if (vStart < vEnd)
                {
                d1d3 = aMax / jMax;								// Maximal duration of phase 1 and 3
                if ((jMax * d1d3 * d1d3) > (vEnd - vStart))		// Do we need a phase 2 ?
                    {
                    d1d3 = sqrtf((vEnd - vStart) / jMax);		// No, so adjust d1d3...
                    aMax = jMax * d1d3;
                    d2 = 0.0f;									// and no need for phase 2
                    dV2 = 0.0f;
                    }
                else
                    {
                    dV2 = (vEnd - vStart) - (jMax * d1d3 * d1d3);	// Yes, phase 1 and 3 are ok, but we need to add a phase 2
                    d2 = dV2 / aMax;
                    }
                s1 = (vStart * d1d3) + jMax * d1d3 * d1d3 * d1d3 / 6.0f;
                s2 = (vStart + vEnd) * d2 * 0.5f;
                s3 = (vEnd * d1d3) - jMax * d1d3 * d1d3 * d1d3 * 0.166666666667f;
                }
            else
                {
                d1d3 = -dMax / jMax;							// Maximal duration of phase 1 and 3
                if ((jMax * d1d3 * d1d3) > (vStart - vEnd))		// Do we need a phase 2 ?
                    {
                    d1d3 = sqrtf((vStart - vEnd) / jMax);		// No, so adjust d1d3...
                    dMax = -jMax * d1d3;
                    d2 = 0.0f;									// and no need for phase 2
                    dV2 = 0.0f;
                    }
                else
                    {
                    dV2 = (vEnd - vStart) + (jMax * d1d3 * d1d3);	// Yes, phase 1 and 3 are ok, but we need to add a phase 2
                    d2 = dV2 / dMax;
                    }
                s1 = (vStart * d1d3) - jMax * d1d3 * d1d3 * d1d3 / 6.0f;
                s2 = (vStart + vEnd) * d2 * 0.5f;
                s3 = (vEnd * d1d3) + jMax * d1d3 * d1d3 * d1d3 * 0.166666666667f;
                break;

            default:
                // Error : unknown/unhandled speedprofile
                break;
                }
        }
    length = calcLength();
    }


// ##################
// ### MotionItem ###
// ##################

void MotionItem::calcSpeedLimits(gCodeParserResult &theParseResult, MachineProperties &theMachineProperties)			// calculate vMax, aMax and dMax from trajectory and machine limits
    {
    vMax = 1000000.0f;
    aMax = 1000000.0f;
    dMax = -1000000.0f;
    double directionUnitVector[(uint8_t)axis::nmbrAxis];

    for (uint8_t i = 0; i < (uint8_t)axis::nmbrAxis; ++i)								// iterate over all Axis
        {
        if ((i != (uint8_t)theParseResult.motion.trajectory.arcAxis[0]) &&
                (i != (uint8_t)theParseResult.motion.trajectory.arcAxis[1]))								// for all Axis, except for the 2 of the arcPlane
            {
            directionUnitVector[i] = theParseResult.motion.trajectory.delta[i] / theParseResult.motion.trajectory.length;		// ### For linear moving Axis, take into account that this Axis is doing only a fraction of the speed of the 3d-move - the projection part..
            }
        else
            {
            directionUnitVector[i] = 1.0;												// ### For circular moving Axis, we don't know the fraction, so we assume worst case

            if (vMax > sqrt(theMachineProperties.motors.aMax[i] * theParseResult.motion.trajectory.radius))
                {
                vMax = (float) sqrt(theMachineProperties.motors.aMax[i] * theParseResult.motion.trajectory.radius);
                }
            }

        if (vMax > fabs(theMachineProperties.motors.vMax[i] / directionUnitVector[i]))						// it seems the floating point division understands 'infinity' and so this works without testing for div by zero
            {
            vMax = (float) fabs(theMachineProperties.motors.vMax[i] / directionUnitVector[i]);				// restrict vMax, if this would result in exceeding vMax for any of the Axis
            }
        if (aMax > fabs(theMachineProperties.motors.aMax[i] / directionUnitVector[i]))
            {
            aMax = (float) fabs(theMachineProperties.motors.aMax[i] / directionUnitVector[i]);			// restrict aMax, if this would result in exceeding aMax for any of the Axis
            }
        if (fabs(dMax) > fabs(theMachineProperties.motors.dMax[i] / directionUnitVector[i]))
            {
            dMax = (float) (-1 * fabs(theMachineProperties.motors.dMax[i] / directionUnitVector[i]));	// restrict dMax, if this would result in exceeding dMax for any of the Axis
            }
        }
    jMax = theMachineProperties.motors.jMax;
    }

float MotionItem::calcTriangular(MotionStrategy theStrategy)						// Calculate the maximum speed for a triangular profile
    {
    switch (theStrategy)
        {
        case MotionStrategy::minimizeSpeed:
            {
            float r;
            r = (2 * sMotion * aMax * dMax) - (vEnd * vEnd * dMax) + (vStart * vStart * aMax);
            if (r >= 0.0f)
                {
                return sqrtf(r / (aMax - dMax));
                }
            else
                {
                return 0.0f;
                }
            }
        break;
        case MotionStrategy::maximizeSpeed:
            // For aMax >0 and dMax <0, the part under de sqrt is always positive
            return sqrtf(((vEnd * vEnd * aMax) - (vStart * vStart * dMax) - (2 * sMotion * aMax * dMax)) / (aMax - dMax));
            break;
        }
    }

float MotionItem::s(float time)
    {
    switch (theMotionProfile)
        {
        case MotionSpeedProfile::zeroOrder:
            return (vMid * time);
            break;

        case MotionSpeedProfile::firstOrder:
            if (time < t3)
                {
                return (vStart * time) + (aMax * time * time * 0.5f);
                }
            else if (time > t4)
                {
                time = time - t4;
                return s4 + (vMid * time) + (dMax * time * time * 0.5f);
                }
            else
                {
                return s3 + (vMid * (time - t3));
                }
            break;

        case MotionSpeedProfile::secondOrder:
            if (time < t3)
                {
                if (time < t1)
                    {
                    return (vStart * time) + (jMax * time * time * time * 0.166666666f);
                    }
                else if (time < t2)
                    {
                    time = time - t2;
                    return s1 + ((vStart + v1v3) * time) + (aMax * time * time * 0.5f);
                    }
                else
                    {
                    time = t3 - time;
                    return s3 - ((vMid * time) - (jMax * time * time * time * 0.166666666f));
                    }
                }
            else if (time > t4)
                {
                if (time < t5)
                    {
                    time = time - t4;
                    return s4 + (vMid * time) - (jMax * time * time * time * 0.166666666f);
                    }
                else if (time < t6)
                    {
                    time = time - t5;
                    return s5 + ((vMid - v5v7) * time) - (dMax * time * time * 0.5f);
                    }
                else
                    {
                    time = tMotion - time;
                    return sMotion - ((vEnd * time) + (jMax * time * time * time * 0.166666666f));
                    }
                }
            else
                {
                return s3 + (vMid * (time - t3));
                }
            break;
        default:
            // TODO : Unknown speedProfile
            break;
        }
    }

float MotionItem::v(float time)
    {
    switch (theMotionProfile)
        {
        case MotionSpeedProfile::zeroOrder:
            return vMid;
            break;
        case MotionSpeedProfile::firstOrder:
            if (time < t3)
                {
                return vStart + (aMax * time);
                }
            else if (time > t4)
                {
                //return vEnd + (dMax * time);
                return vMid + (dMax * (time - t4));
                }
            else
                {
                return vMid;
                }
            break;
        case MotionSpeedProfile::secondOrder:
            if (time < t3)
                {
                if (time < t1)
                    {
                    return vStart + (jMax * time * time * 0.5f);
                    }
                else if (time < t2)
                    {
                    return vStart + v1v3 + (aMax * time);
                    }
                else
                    {
                    return vMid - (jMax * time * time * 0.5f);
                    }
                }
            else if (time > t4)
                {
                if (time < t5)
                    {
                    return vMid - (jMax * time * time * 0.5f);
                    }
                else if (time < t6)
                    {
                    return vMid - v5v7 - (dMax * time);
                    }
                else
                    {
                    return vEnd + (jMax * time * time * 0.5f);
                    }
                }
            else
                {
                return vMid;
                }
            break;
        default:
            // TODO : Unknown speedProfile
            break;
        }
    }

float MotionItem::a(float time)
    {
    switch (theMotionProfile)
        {
        case MotionSpeedProfile::zeroOrder:
            return 0.0f;
            break;
        case MotionSpeedProfile::firstOrder:
            if (time < t3)
                {
                return aMax;
                }
            else if (time > t4)
                {
                return dMax;
                }
            else
                {
                return 0.0f;
                }
            break;
        case MotionSpeedProfile::secondOrder:
            if (time < t3)
                {
                if (time < t1)
                    {
                    return jMax * time;
                    }
                else if (time < t2)
                    {
                    return aMax;
                    }
                else
                    {
                    return jMax * time;
                    }
                }
            else if (time > t4)
                {
                if (time < t5)
                    {
                    return jMax * time;
                    }
                else if (time < t6)
                    {
                    return dMax;
                    }
                else
                    {
                    return jMax * time;
                    }
                }
            else
                {
                return 0.0f;
                }
            break;
        default:
            // TODO : Unknown speedProfile
            break;
        }
    }

void MotionItem::fromParts(MotionPart &left, MotionPart &right, float d4, float len4)
    {
    t1 = left.d1d3;
    t2 = t1 + left.d2;
    t3 = t2 + left.d1d3;
    t4 = t3 + d4;
    t5 = t4 + right.d1d3;
    t6 = t5 + right.d2;
    tMotion = t6 + right.d1d3;
    tStop = tMotion + 1.0f;

    s1 = left.s1;
    s2 = s1 + left.s2;
    s3 = s2 + left.s3;
    s4 = s3 + len4;
    s5 = s4 + right.s1;
    s6 = s4 + right.s2;
    // sMotion is a given for the motionItem, so does not need to be calculated
    }

void MotionItem::toString()
    {
    int32_t vn;
    char sp;
    char sp2;
    float t=0.0f;
    while (t <= tMotion)
        {
        sp = '0';
        if (t > t1)
            {
            sp = '1';
            }
        if (t > t2)
            {
            sp = '2';
            }
        if (t > t3)
            {
            sp = '3';
            }
        if (t > t4)
            {
            sp = '4';
            }
        if (t > t5)
            {
            sp = '5';
            }
        if (t > t6)
            {
            sp = '6';
            }
        if (t >= tMotion)
            {
            sp = '7';
            }

        if (t == 0.0f)
            {
            sp2 = '-';
            }
        else
            {
            sp2 = ' ';
            }

        vn = (int32_t)(v(t)/5) - 1;
        int32_t i = 0;
        while (i < vn)
            {
            logger.log(sp2);
            i++;
            }
        logger.log(sp);
        logger.logNow('\n');
        t = t + 0.25f;
        }
    }

void MotionItem::optimize(MotionStrategy theStrategy, Overrides &theOverrides, MachineProperties &theMachineProperties)			// optimizes this motionItem's speed profile
    {
    switch (theStrategy)
        {
        case MotionStrategy::minimizeSpeed:
            vMid = 0.0f;
            theMotionProfile = MotionSpeedProfile::firstOrder;				// When slowing down, we could slow down faster if we apply firstOrder profile io secondOrder
            break;

        case MotionStrategy::maximizeSpeed:
            vMid = vFeed * theOverrides.feedOverride;						// vFeed was set in gCode, overrides may yield new value, but new vMid must still be within machine limits
            if (vMid > vMax)
                {
                vMid = vMax;
                }
            break;
        }

    MotionPart sectionLeft(theMotionProfile, vStart, vMid, aMax, dMax, theMachineProperties.motors.jMax);				// object calculating the lefthand side section (usually the acceleration phase)
    MotionPart sectionRight(theMotionProfile, vMid, vEnd, aMax, dMax, theMachineProperties.motors.jMax);				// object calculating the righthand side section (usually the deceleration phase)

    sectionLeft.calculate();
    sectionRight.calculate();

    switch (theMotionType)
        {
        case MotionType::Traverse:
            break;
        case MotionType::FeedLinear:
            break;
        case MotionType::FeedHelicalCW:
            break;
        case MotionType::FeedHelicalCCW:
            break;
        case MotionType::PauseAndResume:
            break;
        case MotionType::Pause:
            break;
        case MotionType::None:
            break;
        default:
            break;
        }

    float len4;
    float d4;

    if ((sectionLeft.length + sectionRight.length) > sMotion)
        {
        // Switch to a triangular T-profile
        theMotionProfile = MotionSpeedProfile::firstOrder;
        vMid = calcTriangular(theStrategy);

        sectionLeft.theProfile = MotionSpeedProfile::firstOrder;
        sectionRight.theProfile = MotionSpeedProfile::firstOrder;
        sectionLeft.vEnd = vMid;
        sectionRight.vStart = vMid;
        sectionLeft.calculate();
        sectionRight.calculate();
        len4 = 0.0f;
        d4 = 0.0f;
        }
    else
        {
        // Add a non-zero constant speed mid phase
        len4 = sMotion - (sectionLeft.length + sectionRight.length);
        d4 = len4 / vMid;
        }
    fromParts(sectionLeft, sectionRight, d4, len4);
    }

float MotionItem::calcOtherV(MotionStrategy theStrategy, bool forward)
    {
    // calculates the maximal / minimal entry/exit speed, given length (s), aMax or dMax (adMAx) and vStart or vEnd (v)
    // theStrategy : determines if we want to maximize or minimize the vOther
    // forward : determines if we have vStart as a given, calculating vEnd (true), or vEnd as a given, calculating vStart (false)

    float adMax;	// holds aMax or dMax, depending on the strategy and forward/backward
    float v;		// holds vStart or vEnd, depending on the forward/backward

    if (forward)
        {
        v = vStart;
        }
    else
        {
        v = vEnd;
        }

    switch (theStrategy)
        {
        case MotionStrategy::minimizeSpeed:
            if (forward)
                {
                adMax = dMax;
                }
            else
                {
                adMax = -1 * aMax;
                }
            break;
        case MotionStrategy::maximizeSpeed:
            if (forward)
                {
                adMax = aMax;
                }
            else
                {
                adMax = -1 * dMax;
                }
            break;
        default:
            break;
        }

    switch (theMotionProfile)
        {
        case MotionSpeedProfile::zeroOrder:
            // Todo
            break;

        case MotionSpeedProfile::firstOrder:
            // in case we decelerate, will we reach vOther = 0 in less distance than s ??
            float d;
            d = (v * v) + (2.0f * sMotion * adMax);
            if (d > 0.0f)
                {
                return sqrt(d);
                }
            else
                {
                return 0.0f;
                }
            break;

        case MotionSpeedProfile::secondOrder:
            // Todo
            return 0.0f;
            break;
        }
    }


// ##################
// ### Motion     ###
// ##################
// This class implements a single motion controller
// Contains a buffer of motionItems, and related readindex and bufferlevel
// Contains a buffer of stepperItem, and related readindex and bufferLevel

//Motion::Motion(EventBuffer &theEventbuffer)

//Motion::Motion() : theEventBuffer(theEventBuffer)
//    {
//    }

Motion::Motion(EventBuffer &theEventbuffer) : theEventBuffer(theEventBuffer)
    {
#ifdef log_motion_class
    logger.logNow("motion constructor start\n");
#endif

    // Initialize the stepBuffer with some default values
    stepBufferReadIndex = 0;
    for (uint32_t i = 0; i < minStepBufferItems; i++)
        {
        stepBuffer[i].out = 0x00;
        stepBuffer[i].reload = defaultReload;
        stepBufferTotalTime = stepBufferTotalTime + defaultReload;
        stepBufferLevel++;
        }

#ifdef log_motion_class
    logger.logNow("stepBuffer initialized\n");
#endif


#ifndef WIN32
    // Setup the Periodic interrupt Timer
    SIM_SCGC6 |= SIM_SCGC6_PIT;											// Activates the clock for PIT0, PIT1 and PIT2
    PIT_MCR = 0x00;														// PIT Module Control Register (PIT_MCR) This register enables or disables the PIT timer clocks and controls the timers when the PIT enters the Debug mode.

#ifdef log_mainController_class
    logger.logNow("Enabling Clock to PIT\n");
#endif

    // PIT0 and PIT1 are used for outputting stepper (and peripherals) signals with the correct timing
    // PIT0 is used as a prescaler to divide the clock by (eg) 750,
    // PIT1 is chained to PIT0 and used to time the stepper signals

    PIT_LDVAL0 = preScaler - 1;											// reloadValue of X results in a X+1 bus cycle period
//	PIT_LDVAL0 = (preScaler*200) - 1;											// debugging only, slowing it down to 1 reload per second in wait..
    PIT_TCTRL0 = 0x1;													// Timer Control Register (PIT_TCTRLn) 	Start this timer, but don't do interrupts

    PIT_LDVAL1 = defaultReload - 1;										// Timer Load Value Register (PIT_LDVALn) 	These registers select the timeout period for the timer interrupts.
    NVIC_ENABLE_IRQ(IRQ_PIT_CH1);										// Enable PIT1 interrupt in the vector-table
    PIT_TCTRL1 = 0x7;													// Timer Control Register (PIT_TCTRLn). Chain the timer to PIT0, Start the timer and enable interrupts

#ifdef log_mainController_class
    logger.logNow("Enabling PIT0, PIT1, Stepper Signals Timing\n");
#endif

    // PIT2 is used with 5ms periodic interrupts for capturing buttons and limit switches
    PIT_LDVAL2 = (F_BUS / 200) - 1;										// Timer Load Value Register (PIT_LDVALn)
//	PIT_LDVAL2 = F_BUS - 1;												// Timer Load Value Register (PIT_LDVALn)
    NVIC_ENABLE_IRQ(IRQ_PIT_CH2);										// Enable PIT2 interrupt in the vector-table
    PIT_TCTRL2 = 0x3;													// Timer Control Register (PIT_TCTRLn) 	These registers contain the control bits for each timer.start timer and enable interrupts

#ifdef log_mainController_class
    logger.logNow("Enabling PIT2, Inputs sampling\n");
#endif

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
    PORTC_PCR0 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC0 into Alt 1, High Drive Strength
    PORTC_PCR1 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC1 into Alt 1, High Drive Strength
    PORTC_PCR2 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC2 into Alt 1, High Drive Strength
    PORTC_PCR3 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC3 into Alt 1, High Drive Strength
    PORTC_PCR4 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC4 into Alt 1, High Drive Strength
    PORTC_PCR5 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortC5 into Alt 1, High Drive Strength
    //PORTC_PCR6 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //PORTC_PCR7 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //PORTC_PCR8 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //PORTC_PCR9 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //PORTC_PCR10 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //PORTC_PCR11 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    GPIOC_PDDR = (0x3F);												// Set PortC[5..0] as Output
    //GPIOC_PDDR = 0x00000FFF;											// Set PortC[0..11] as Output

#ifdef log_mainController_class
    logger.logNow("PortC[0..5] set as output\n");
#endif

    // Stepper Driver Enable signals
    GPIOB_PDDR = 0x000C0000;											// Set PortB[18..19] as Output
    PORTB_PCR18 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortB[18..19] into Alt 1, High Drive Strength																		// Enable for Motors [1..3] : PB18
    PORTB_PCR19 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    //GPIOB_PDOR = 0;					// Enable Motors 123 and 456 ?
    GPIOB_PDOR = 0x40000;		// Disable Motors 123 Apparently driving the signal HIGH, Disables the motors, keeping it LOW enables them

#ifdef log_mainController_class
    logger.logNow("PortB[18..19] set as output - motors disabled\n");
#endif
#endif

    theOverrides.feedOverride = 1.0f;			// standard feedOverride is 100% of Gcode speed
    theOverrides.spindleOverride = 1.0f;		// idem for Spindle
#ifdef log_motion_class
    logger.logNow("motion constructor end\n");
#endif
    }

bool Motion::isReady() const										// Is there enough free space in the motionBuffer ?
    {
    //return(motionBufferLevel < motionBufferTreshold);
    return(motionBufferLevel > 0);
    };

void Motion::append(gCodeParserResult &theParseResult)
    {
    if (motionBufferLevel < motionBufferLength)
        {
        // 1. Append a new MotionItem at the tail of the buffer
        MotionItem* newItem;
        newItem = motionBuffer + ((motionBufferReadIndex + motionBufferLevel) % motionBufferLength);		// Set pointer to the being appended MotionItem
        motionBufferLevel++;

        newItem->theMotionType = theParseResult.motion.theMotionType;
        switch (newItem->theMotionType)
            {
            case MotionType::Traverse:
            case MotionType::FeedLinear:
            case MotionType::FeedHelicalCW:
            case MotionType::FeedHelicalCCW:
                break;

            case MotionType::PauseAndResume:
            case MotionType::Pause:

                break;
            case MotionType::None:
                break;
            }

        //	2.	Most of the trajectory & peripherals parameters just need to be converted to float and copied
            {

            newItem->arcAxis0 = theParseResult.motion.trajectory.arcAxis[0];
            newItem->arcAxis1 = theParseResult.motion.trajectory.arcAxis[1];
            newItem->arcCenter0 = (float)theParseResult.motion.trajectory.arcCenter[0];
            newItem->arcCenter1 = (float)theParseResult.motion.trajectory.arcCenter[1];
            newItem->startAngle = (float)theParseResult.motion.trajectory.startAngle;
            newItem->radius = (float)theParseResult.motion.trajectory.radius;

            newItem->spindledirection = theParseResult.motion.peripherals.spindledirection;
            newItem->spindleSpeed = (float)theParseResult.motion.peripherals.spindleSpeed;
            newItem->coolantFlood = theParseResult.motion.peripherals.coolantFlood;
            newItem->coolantMist = theParseResult.motion.peripherals.coolantMist;

            newItem->sMotion = (float)theParseResult.motion.trajectory.length;
            newItem->vFeed = (float)theParseResult.motion.speedProfile.vFeed;
            }

        //	3.	Calculate for each Axis how big is the movement component
            {
            for (uint8_t i = 0; i < (uint8_t)axis::nmbrAxis; ++i)
                {
                if ((i == (uint8_t)theParseResult.motion.trajectory.arcAxis[0]) || (i == (uint8_t)theParseResult.motion.trajectory.arcAxis[1]))
                    {
                    newItem->deltaRealTime[i] = (float)(theParseResult.motion.trajectory.deltaAngle / theParseResult.motion.trajectory.length);					// for the ARC part
                    }
                else
                    {
                    newItem->deltaRealTime[i] = (float)(theParseResult.motion.trajectory.delta[i] / theParseResult.motion.trajectory.length);					// for the LINEAR part
                    }
                newItem->startPosition[i] = (float)theParseResult.motion.trajectory.startPosition[i];
                }
            }

        //	4.	Now calculate all parameters needed to (real-time) execute the speed-profile, within all constraints, overrides, etc
            {
            switch (newItem->theMotionType)
                {
                case MotionType::Pause:
                case MotionType::PauseAndResume:
                    newItem->theMotionProfile = MotionSpeedProfile::firstOrder;
                    newItem->tMotion = (float)theParseResult.motion.speedProfile.duration;
                    break;

                default:
                    if (theParseResult.motion.trajectory.length >= theMachineProperties.minLengthSProfile)
                        {
                        newItem->theMotionProfile = MotionSpeedProfile::secondOrder;
                        }
                    else
                        {
                        newItem->theMotionProfile = MotionSpeedProfile::firstOrder;
                        }
                    // 1. For the time being, the new motion starts and ends at speed zero
                    newItem->vStart = 0.0f;
                    newItem->vEnd = 0.0f;

                    // 2. Calculate the Machine's speed limitations for this individual motion
                    newItem->calcSpeedLimits(theParseResult, theMachineProperties);
                    // 3. Optimize this motion, as we know for sure it's not optimal yet. Further iterations of optimize will further optimize this and all older motionItems
                    newItem->optimize(theStrategy, theOverrides, theMachineProperties);
                    break;
                }
            }
        }
    else
        {
        // Error : motonBuffer overflow..
        }
    }

void Motion::optimize()
    {
    switch (motionBufferLevel)
        {
        case 0:									// empty motionBuffer - nothing to do as there is nothing to optimize
            break;

        case 1:									// only 1 item, which is currently being executed -> derive a new motion for the remaining part, and optimize it's speed
            motionBuffer[motionBufferReadIndex].adjustRemaining();			// TODO : create new item from remaining part of motion...
            motionBuffer[motionBufferReadIndex].optimize(theStrategy, theOverrides, theMachineProperties);
            break;

        default:								// 2 or more items -> optimize them in pairs, each call of this function optimizes 1 pair.
            // minimizing is done by taking the pairs from left to right = oldest to newest.
            // maximizing is done by taking the pairs from right to left = newest to oldest
            // As only the junction speed of a pair is modified (not the entry/exit speed of the pair) it takes several iterations over the complete buffer to arrive a full optimization
            uint8_t itemLeft;					// index of the left MotionItem (oldest)
            uint8_t itemRight;					// index of the right MotionItem (newest)
            float vJunction;					// exit-entry speed between the motion-pair after optimizing
            float v;							// local variable to calculate and test for speed conditions

            switch (theStrategy)
                {
                case MotionStrategy::minimizeSpeed:
                    // minimizing speed -> iterate from left to right over motions
                    if (junctionIndex > motionBufferLevel - 2)
                        {
                        junctionIndex = 0;
                        }
                    else
                        {
                        junctionIndex++;
                        }
                    break;

                case MotionStrategy::maximizeSpeed:
                    // maximize speed -> iterate from right to left over motions
                    if (0 == junctionIndex)
                        {
                        junctionIndex = motionBufferLevel - 2;
                        }
                    else
                        {
                        junctionIndex--;
                        }
                    break;
                }

            itemLeft = (motionBufferReadIndex + junctionIndex) % motionBufferLength;
            itemRight = (motionBufferReadIndex + junctionIndex + 1) % motionBufferLength;

            if ((itemLeft == motionBufferReadIndex) || (itemRight == motionBufferReadIndex))
                {
                motionBuffer[motionBufferReadIndex].adjustRemaining();			// create new item from remaining part of motion...
                }

            switch (theStrategy)
                {
                case MotionStrategy::minimizeSpeed:
                    vJunction = 0; // assume we can slow down to zero, adjust upwards if needed by other constraints

                    v = motionBuffer[itemLeft].calcOtherV(MotionStrategy::minimizeSpeed, true);
                    if (v > vJunction)
                        {
                        vJunction = v;
                        }

                    v = motionBuffer[itemRight].calcOtherV(MotionStrategy::minimizeSpeed, false);
                    if (v > vJunction)
                        {
                        vJunction = v;
                        }
                    break;

                case MotionStrategy::maximizeSpeed:
                    // calculate the maximal vEnd/vStart = vJunction, starting from a very large value, then restricting it down so it meets all kind of constraints
                    vJunction = 999999999; // some large value to be restricted down... TODO : makes this cleaner eg using HUGE_VALF

                    // restrict to maximal vEnd for given vStart of the lefthand motionItem
                    v = motionBuffer[itemLeft].calcOtherV(MotionStrategy::maximizeSpeed, true);
                    if (v < vJunction)
                        {
                        vJunction = v;
                        }

                    // restrict to maximal vStart for given vEnd of the righthand motionItem
                    v = motionBuffer[itemRight].calcOtherV(MotionStrategy::maximizeSpeed, false);
                    if (v < vJunction)
                        {
                        vJunction = v;
                        }

                    // restrict to respect feedrates and machine limits of both left and right
                    v = motionBuffer[itemLeft].vFeed * theOverrides.feedOverride;
                    if (v < vJunction)
                        {
                        vJunction = v;
                        }

                    v = motionBuffer[itemLeft].vMax;
                    if (v < vJunction)
                        {
                        vJunction = v;
                        }

                    v = motionBuffer[itemRight].vFeed * theOverrides.feedOverride;
                    if (v < vJunction)
                        {
                        vJunction = v;
                        }

                    v = motionBuffer[itemRight].vMax;
                    if (v < vJunction)
                        {
                        vJunction = v;
                        }

                    // TODO : vJunction needs to be further reduced according to the kind of corners the trajectory is taking...
                    break;
                }

            // now with this optimal junction speed, recalculate both motionItems
            motionBuffer[itemLeft].vEnd = vJunction;
            motionBuffer[itemRight].vStart = vJunction;
            motionBuffer[itemLeft].optimize(theStrategy, theOverrides, theMachineProperties);
            motionBuffer[itemRight].optimize(theStrategy, theOverrides, theMachineProperties);
        }
    }

stepperItem Motion::getNextStep()
    {
    while (true)
        {
        timeInMotionTicks++;																// Determine sampling time as 'previous sample time' + sampling interval (1)
        lastReload++;
        while (true)
            {
            if (motionBufferLevel > 0)
                {
                MotionItem* currentMotionItem = &motionBuffer[motionBufferReadIndex];		// pointer to active motion, speeds up accessing its members
                float timeInMotionFloat;													// equivalent in floating point seconds
                timeInMotionFloat = (float)timeInMotionTicks * timerTick;					// convert discrete time in timerticks to float time in seconds

                if (timeInMotionFloat > currentMotionItem->tMotion)
                    {
                    timeInMotionTicks = timeInMotionTicks - (uint32_t)((currentMotionItem->tMotion) * timerFrequency);
                    //timeInMotionFloat = timeInMotionFloat - currentMotionItem->tMotion;

                    motionBufferReadIndex = (motionBufferReadIndex + 1) % motionBufferLength;
                    --motionBufferLevel;

                    //pushEvent(Event::limitSwitchXMaxOpened);	//fire a motionComplete event();
                    continue;
                    }
                else
                    {
                    float sNow = currentMotionItem->s(timeInMotionFloat);

                    uint32_t mask = 0x00000001;	// initialize bitmask needed for manipulating Step and Dir bits
                    // 4. For each axis..
                    for (uint8_t i = 0; i < (uint8_t)axis::nmbrAxis; ++i)
                        {
                        // 4a. Determine the position for this axis for this distance travelled
                        if (currentMotionItem->deltaRealTime[i] != 0.0f)			// Optimization : only do calculations for Axis that are moving
                            {
                            if (i == (uint8_t)currentMotionItem->arcAxis0)
                                {
                                nextPositionInMm[i] = (currentMotionItem->arcCenter0 + (currentMotionItem->radius * cosf(currentMotionItem->startAngle + (currentMotionItem->deltaRealTime[i] * sNow))));
                                }
                            else if (i == (uint8_t)currentMotionItem->arcAxis1)
                                {
                                nextPositionInMm[i] = (currentMotionItem->arcCenter1 + (currentMotionItem->radius * sinf(currentMotionItem->startAngle + (currentMotionItem->deltaRealTime[i] * sNow))));
                                }
                            else
                                {
                                nextPositionInMm[i] = (currentMotionItem->startPosition[i] + currentMotionItem->deltaRealTime[i] * sNow);
                                }

                            // 4b. Determine if this new position requires a step, forward or backwards
                            // NOTE - TODO there is some problem here as the conversion from float to int rounds towards zero (discards the fractional part) which means the interval [-1,1] takes twice the time..
                            // Should test this on the K64/K66 to see how it behaves there
                            if ((int32_t)((nextPositionInMm[i] * theMachineProperties.motors.stepsPerMm[i]) - hysteresis) > currentPositionInSteps[i])         // Determine if this position involves step forward...
                                {
                                // Step forward
                                //logger.logNow("Step++\n");
                                dirSetup = dirSetup & ~mask;			// mask step signal to zero (should already be zero)
                                stepRise = stepRise | mask;				// set step pulse high
                                stepFall = stepFall & ~mask;			// return step pulse low
                                mask = mask << 1;						// adjust mask for direction bits
                                dirSetup = dirSetup | mask;				// set direction high
                                stepRise = stepRise | mask;				// set direction high
                                stepFall = stepFall | mask;				// set direction high
                                mask = mask << 1;						// adjust mask for next iteration = next axis
                                currentPositionInSteps[i]++;
                                }
                            else if ((int32_t)((nextPositionInMm[i] * theMachineProperties.motors.stepsPerMm[i]) + hysteresis) < currentPositionInSteps[i])		// ... or a step backwards
                                {
                                // Step backwards
                                dirSetup = dirSetup & ~mask;			// mask step signal to zero (should already be zero)
                                stepRise = stepRise | mask;				// set step pulse high
                                stepFall = stepFall & ~mask;			// return step pulse low
                                mask = mask << 1;						// adjust mask for direction bits
                                dirSetup = dirSetup & ~mask;			// set direction low
                                stepRise = stepRise & ~mask;			// set direction low
                                stepFall = stepFall & ~mask;			// set direction low
                                mask = mask << 1;						// adjust mask for next iteration = next axis
                                currentPositionInSteps[i]--;
                                }
                            else
                                {
                                mask = mask << 2;
                                }
                            }
                        else
                            {
                            mask = mask << 2;
                            }
                        }
                    if ((dirSetup != lastOut) || (lastReload >= defaultReload))
                        {
                        lastOut = dirSetup;
                        stepperItem output;
                        output.out = dirSetup;
                        output.reload = lastReload-1;
                        lastReload = 0;

                        // Shift the bitpatterns for the next run..
                        dirSetup = stepRise;
                        stepRise = stepFall;
                        return output;
                        }
                    }
                }
            else
                {
                // no more motionItems in the buffer -> repeat whatever is already written out
                timeInMotionTicks = 0;
                stepperItem output;
                output.out = lastOut;
                output.reload = defaultReload-1;
                return output;
                }
            break;
            }
        // shift the bitpatterns for the next run..
        dirSetup = stepRise;
        stepRise = stepFall;
        }
    }

void Motion::fill(stepperItem anItem)
    {
    if (stepBufferLevel < stepBufferLength)																// only if buffer not full
        {
        uint16_t stepBufferWriteIndex;
        //disable_interrups();		// lock
        stepBufferTotalTime = stepBufferTotalTime + anItem.reload;										// adjust total time in buffer
        if (stepBufferLevel > 2)																		// This is the normal case, as we try to keep the stepBuffer filled with enough steps
            {
            stepBufferWriteIndex = (stepBufferReadIndex + stepBufferLevel - 2) % stepBufferLength;		// Calculate writeIndex for TimerReload as offset from readIndex. This is actually 2 positions back in the buffer
            stepBuffer[stepBufferWriteIndex].reload = anItem.reload;									// write the TimerReload
            stepBufferWriteIndex = (stepBufferWriteIndex + 2) % stepBufferLength;						// Calculate writeIndex for Outputs
            stepBuffer[stepBufferWriteIndex].out = anItem.out;											// write the Output bits
            stepBufferLevel++;																			// adjust the bufferLevel
            }
        else																							// This is the startup case, buffer is empty but we have motions to fill it
            {
            // this should never happen... but if it does how do we handle it
            }
        //enable_interrups();		// unlock
        }
    }

void Motion::output()
    {
    if (stepBufferLevel >= minStepBufferItems)
        {
#ifndef WIN32
        PIT_LDVAL1 = stepBuffer[stepBufferReadIndex].reload;									// reload timer
        GPIOC_PDOR = stepBuffer[stepBufferReadIndex].out;

        //logger.log("output:");
        //logger.log(stepBuffer[stepBufferReadIndex].out);
        //logger.logNow("\n");

#endif
//        stepBufferTotalTime = stepBufferTotalTime - stepBuffer[stepBufferReadIndex].reload;		// adjust total time in buffer
        stepBufferReadIndex = (stepBufferReadIndex + 1) % stepBufferLength;						// adjust read pointer to next position
        stepBufferLevel--;																		// adjust stepBufferLevel to one less than before
        }
    else
        {
        // buffer underrun... should not happen
        }
    }

void Motion::run()
    {
    // 1. Fill the buffer when needed
    while ((stepBufferLevel < minStepBufferItems) || ((stepBufferTotalTime < minStepBufferTotalTime) && (stepBufferLevel < stepBufferLength)))
        {
        stepperItem anItem = getNextStep();				// get next step from Motion...
        fill(anItem);									// ... and pump it to stepBuffer
        }

#ifdef WIN32
    // 2. Only in Windows, output from the buffer instead of bufferRead being triggered from timer interrupt
    while (stepBufferLevel >= minStepBufferItems)
        {
        output();
        }
#endif
    }

void Motion::toString()
    {
    for (uint32_t i = 0; i < motionBufferLevel; i++)
        {
        motionBuffer[(motionBufferReadIndex + i) % motionBufferLength].toString();
        }


    //for (uint16_t i = 0; i < stepBufferLength; i++)
    //    {
    //    snprintf(logger.logLine, 127, "%x %d \n", stepBuffer[i].out, stepBuffer[i].reload);
    //    logger.logNow();
    //    }
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

