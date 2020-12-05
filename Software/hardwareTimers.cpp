#include "hardwareTimers.h"
#include "machineproperties.h"
#include "logging.h"

extern uLog theLog;

hardwareTimers::hardwareTimers() {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6

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
#endif
    theLog.output(loggingLevel::Debug, "hardwareTimers initialized");
}

void hardwareTimers::enableInputTimer(bool onOff) {
    if (onOff) {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
        PIT_TFLG2  = 0x1;                                   // clear timer interrupt flag
        PIT_TCTRL2 = 0x3;                                   // Timer Control Register (PIT_TCTRLn) 	These registers contain the control bits for each timer.start timer and enable interrupts
#endif
        theLog.output(loggingLevel::Debug, "inputTimer started");
    } else {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
        PIT_TCTRL2 = 0x0;                                   // Timer Control Register (PIT_TCTRLn) 	These registers contain the control bits for each timer.start timer and enable interrupts
        PIT_TFLG2  = 0x1;                                   // clear timer interrupt flag
#endif
        theLog.output(loggingLevel::Debug, "inputTimer stopped");
    }
}

void hardwareTimers::enableOutputTimer(bool onOff) {
    if (onOff) {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
        PIT_TFLG1  = 0x1;                                   // Clear timer interrupt flag for PIT1
        PIT_TCTRL0 = 0x1;                                   // Enable the Timer PIT0
        PIT_TCTRL1 = 0x7;                                   // Timer Control Register (PIT_TCTRLn). Chain the timer to PIT0, Start the timer and enable interrupts
#endif
        theLog.output(loggingLevel::Debug, "outputTimer started");
    } else {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
        PIT_TCTRL1 = 0x4;                                   // Disable the Timer PIT1 and its Interrupts
        PIT_TCTRL0 = 0x0;                                   // Disable the Timer PIT0
        PIT_TFLG1  = 0x1;                                   // Clear timer interrupt flag for PIT1
#endif
        theLog.output(loggingLevel::Debug, "outputTimer stopped");
    }
}

void hardwareTimers::setOutputTimerReload(uint32_t value) {
#if defined(__MK64FX512__) || defined(__MK66FX1M0__)        // Teensy 3.5 || Teensy 3.6
    PIT_LDVAL1 = value;                                     // TODO think about using value or value - 1, what should be most logical and simple ?
#endif
}
