// #############################################################################
// ### This file is part of the source code for the Moovr CNC Controller     ###
// ### https://github.com/Strooom/Moovr                                      ###
// ### Author(s) : Pascal Roobrouck - @strooom                               ###
// ### License : https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode ###
// #############################################################################

#include "stepper.h"

Stepper::Stepper()
    {
    initialize();

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
    PORTC_PCR6 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    PORTC_PCR7 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    PORTC_PCR8 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    PORTC_PCR9 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    PORTC_PCR10 = PORT_PCR_MUX(1) | PORT_PCR_DSE;
    PORTC_PCR11 = PORT_PCR_MUX(1) | PORT_PCR_DSE;

    GPIOC_PDDR = 0x00000FFF;											// Set PortC[0..11] as Output

    // Stepper Driver Enable signals
    GPIOB_PDDR = 0x000C0000;											// Set PortB[18..19] as Output
    PORTB_PCR18 = PORT_PCR_MUX(1) | PORT_PCR_DSE;						// Set PortB[18..19] into Alt 1, High Drive Strength		// Enable for Motors [1..3] : PB18
    PORTB_PCR19 = PORT_PCR_MUX(1) | PORT_PCR_DSE;

    // Setup the Periodic interrupt Timers
    // PIT0 and PIT1 are used for outputting stepper (and peripherals) signals with the correct timing
    // PIT0 is used as a prescaler to divide the clock,
    // PIT1 is chained to PIT0 and used to time the stepper signals
    SIM_SCGC6 |= SIM_SCGC6_PIT;											// Activates the clock for PIT0, PIT1 and PIT2
    PIT_MCR = 0x00;														// PIT Module Control Register (PIT_MCR) This register enables or disables the PIT timer clocks and controls the timers when the PIT enters the Debug mode.

    PIT_LDVAL0 = (pit0PreScaler - 1);									// reloadValue of X results in a X+1 bus cycle period
    PIT_TCTRL0 = 0x0;													// Timer Control Register (PIT_TCTRLn). Do not yet start this timer. It can be done by calling start();

    PIT_LDVAL1 = defaultReload - 1;										// Timer Load Value Register (PIT_LDVALn) 	These registers select the timeout period for the timer interrupts.
    PIT_TCTRL1 = 0x0;													// Timer Control Register (PIT_TCTRLn). Chain the timer to PIT0, Start the timer and enable interrupts
    NVIC_ENABLE_IRQ(IRQ_PIT_CH1);										// Enable PIT1 interrupt in the vector-table
    }

void Stepper::initialize()
    {
    cli();											// begin critical section
    stepBufferLevel = 0;							// number of items in the buffer
    stepBufferHead = 0;								// index of next item to read
    stepBufferTotalTime = 0;						// total amount of timer cycles in buffer.. we try to keep this at ~ 10 ms
    lastError = stepperError::none;					// if any error occurs, we will remember it here

    // now populate the buffer with some minimum items
    for (uint32_t i = 0; i < minStepBufferItems; i++)
        {
        stepBuffer[i].out = 0x00;
        stepBuffer[i].reload = defaultReload;
        stepBufferTotalTime = stepBufferTotalTime + defaultReload;
        stepBufferLevel++;
        }
    lastError = stepperError::none;
    sei();											// end critical section
    }

void Stepper::start()
    {
    PIT_TFLG1 = 0x1;						// Clear timer interrupt flag for PIT1
    PIT_TCTRL0 = 0x1;						// Enable the Timer PIT0
    PIT_TCTRL1 = 0x7;						// Timer Control Register (PIT_TCTRLn). Chain the timer to PIT0, Start the timer and enable interrupts
    }

void Stepper::stop()
    {
    PIT_TCTRL1 = 0x4;						// Disable the Timer PIT1 and its Interrupts
    PIT_TCTRL0 = 0x0;						// Disable the Timer PIT0
    PIT_TFLG1 = 0x1;						// Clear timer interrupt flag for PIT1
    }

void Stepper::fill()
    {
    while ((stepBufferLevel < minStepBufferItems) || ((stepBufferTotalTime < minStepBufferTotalTime) && (stepBufferLevel < stepBufferLength)))
        {
        stepperItem anItem = getNextStep();																// get next step from Motion...
        uint32_t stepBufferWriteIndex;
        cli();																							// critical section : data shared with interrupt handler
        stepBufferTotalTime = stepBufferTotalTime + anItem.reload;										// adjust total time in buffer
        if (stepBufferLevel > 2)																		// This is the normal case, as we try to keep the stepBuffer filled with enough steps
            {
            stepBufferWriteIndex = (stepBufferHead + stepBufferLevel - 2) % stepBufferLength;			// Calculate writeIndex for TimerReload as offset from readIndex. This is actually 2 positions back in the buffer
            stepBuffer[stepBufferWriteIndex].reload = anItem.reload;									// write the TimerReload
            stepBufferWriteIndex = (stepBufferWriteIndex + 2) % stepBufferLength;						// Calculate writeIndex for Outputs
            stepBuffer[stepBufferWriteIndex].out = anItem.out;											// write the Output bits
            stepBufferLevel++;																			// adjust the bufferLevel
            }
        else																							// This is the startup case, buffer is empty but we have motions to fill it
            {
            lastError = stepperError::bufferUnderRun2;													// this should never happen... but if it does, we remember it.
            }
        sei();																							// end critical section
        }
    }

void Stepper::output()
    {
    if (stepBufferLevel >= minStepBufferItems)
        {
        PIT_LDVAL1 = stepBuffer[stepBufferHead].reload;													// reload timer
        GPIOC_PDOR = stepBuffer[stepBufferHead].out;

        if (stepBufferTotalTime > stepBuffer[stepBufferHead].reload)									// stepBufferTotalTime should not go negative...
            {
            stepBufferTotalTime = stepBufferTotalTime - stepBuffer[stepBufferHead].reload;				// adjust total time in buffer
            }
        else
            {
            lastError = stepperError::bufferTotalTimeError;
            }

        stepBufferHead = (stepBufferHead + 1) % stepBufferLength;										// adjust read pointer to next position
        stepBufferLevel--;																				// adjust stepBufferLevel to one less than before
        }
    else
        {
        lastError = stepperError::bufferUnderRun1;														// buffer underrun... should not happen
        }
    }

stepperError Stepper::getLastError()
    {
    stepperError tmpError = lastError;
    lastError = stepperError::none;
    return tmpError;
    }

void Stepper::enableMotors123()
    {
    GPIOB_PCOR = (0x00000001 << 18);
    }

void Stepper::enableMotors456()
    {
    GPIOB_PCOR = (0x00000001 << 19);
    }

void Stepper::disableMotors123()
    {
    GPIOB_PSOR = (0x00000001 << 18);
    }

void Stepper::disableMotors456()
    {
    GPIOB_PSOR = (0x00000001 << 19);
    }

stepperItem Stepper::getNextStep()		// Stub to generate some stepper data without needing all of the other motion control stuff
    {
    stepperItem newItem;
    ix++;
    switch (ix % 8)
        {
        case 0:
			newItem.out = 0x000002;
			newItem.reload = 99;
			break;

		case 1:
			newItem.out = 0x000003;
			newItem.reload = 0;
			break;

		case 2:
			newItem.out = 0x000002;
			newItem.reload = 0;
			break;

		case 3:
			newItem.out = 0x000003;
			newItem.reload = 19;
			break;

		case 4:
			newItem.out = 0x000002;
			newItem.reload = 0;
			break;

		case 5:
			newItem.out = 0x000003;
			newItem.reload = 19;
			break;

		case 6:
			newItem.out = 0x000002;
			newItem.reload = 0;
			break;

		case 7:
			newItem.out = 0x000000;
			newItem.reload = 1000;
			break;
	}
    return newItem;
    }
