#include "timers.h"
#include "machineproperties.h"

// ---------------------------------------------------------------------------------------
// Implementation for Teensy 3.5 (MK64FX512 MCU) and Teensy 3.6 (MK66FX1M0 MCU) 
// ---------------------------------------------------------------------------------------

#if defined(__MK64FX512__) || defined(__MK66FX1M0__)

hardwareTimers::hardwareTimers() {
    // ---------------------------------------------------------------------------------------
    // Setting up clocks to enable the K64/K66 Programmable Interval Timer (PIT) functionality
    // ---------------------------------------------------------------------------------------

    SIM_SCGC6 |= SIM_SCGC6_PIT;        // Activates the clock for PIT0, PIT1 and PIT2
    PIT_MCR = 0x00;                    // PIT Module Control Register (PIT_MCR) This register enables or disables the PIT timer clocks and controls the timers when the PIT enters the Debug mode.

    // --------------------------------------------------------------------------
    // Setting up PIT0 and PIT1, chained to provide timing for the stepper motors
    // --------------------------------------------------------------------------

    uint32_t pit0PreScaler = busFrequency / outputTimerFrequency;        // dividing the bus frequency (going into PIT0) which then goes into PIT1 through PIT0 chaining
    PIT_LDVAL0             = (pit0PreScaler - 1);                        // reloadValue of X-1 results in a X bus-cycle period
    PIT_TCTRL0             = 0x0;                                        // Timer Control Register (PIT_TCTRLn)

    PIT_LDVAL1 = 100;                    // Timer Load Value Register (PIT_LDVALn) 	These registers select the timeout period for the timer interrupts.
    PIT_TCTRL1 = 0x0;                    // Timer Control Register (PIT_TCTRLn). Chain the timer to PIT0
    NVIC_ENABLE_IRQ(IRQ_PIT_CH1);        // Enable PIT1 interrupt in the vector-table

    // -----------------------------------------------------------------------------
    // Setting up PIT2 to provide timing for sampling the input switches and buttons
    // -----------------------------------------------------------------------------

    SIM_SCGC6 |= SIM_SCGC6_PIT;        // Activates the clock for PIT0, PIT1 and PIT2
    PIT_MCR = 0x00;                    // PIT Module Control Register (PIT_MCR) This register enables or disables the PIT timer clocks and controls the timers when the PIT enters the Debug mode.

    PIT_LDVAL2 = pit2Reload;             // Timer Load Value Register (PIT_LDVALn)
    NVIC_ENABLE_IRQ(IRQ_PIT_CH2);        // Enable PIT2 interrupt in the vector-table
    PIT_TCTRL2 = 0x0;                    // Timer Control Register (PIT_TCTRLn) 	These registers contain the control bits for each timer. Do not yet start timer and enable interrupts, will call start() later
}

void hardwareTimers::enableInputTimer(bool onOff) {
    if (onOff) {
        PIT_TFLG2  = 0x1;        // clear timer interrupt flag
        PIT_TCTRL2 = 0x3;        // Timer Control Register (PIT_TCTRLn) 	These registers contain the control bits for each timer.start timer and enable interrupts
    } else {
        PIT_TCTRL2 = 0x0;        // Timer Control Register (PIT_TCTRLn) 	These registers contain the control bits for each timer.start timer and enable interrupts
        PIT_TFLG2  = 0x1;        // clear timer interrupt flag
    }
}

void hardwareTimers::enableOutputTimer(bool onOff) {
    if (onOff) {
        PIT_TFLG1  = 0x1;        // Clear timer interrupt flag for PIT1
        PIT_TCTRL0 = 0x1;        // Enable the Timer PIT0
        PIT_TCTRL1 = 0x7;        // Timer Control Register (PIT_TCTRLn). Chain the timer to PIT0, Start the timer and enable interrupts
    } else {
        PIT_TCTRL1 = 0x4;        // Disable the Timer PIT1 and its Interrupts
        PIT_TCTRL0 = 0x0;        // Disable the Timer PIT0
        PIT_TFLG1  = 0x1;        // Clear timer interrupt flag for PIT1
    }
}

void hardwareTimers::setOutputTimerReload(uint32_t value) {
    PIT_LDVAL1 = value;        //
}

#endif