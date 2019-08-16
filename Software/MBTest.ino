#include <Arduino.h>
//#include "hostInterface.h"
#include "mainController.h"
//#include "gCode.h"
#include "logging.h"

#ifdef  log_all
Logger logger;													// one instance, constructed as first, as this constructor needs to be the first code which is executed, in order for all subsequent code to have the debug-output available
#endif //  log_all

MainController theMainController;										// one instance, global scope object
//uint32_t out = 0; // for testing only

void setup()															// nothing is done here, it all happens in the constructors of the global objects, and the embedded objects
    {
    Serial.begin(38400);
    delay(100);
    Serial.println("USB Serial Port initialized\n");
    logger.outputIsAvailable = true;
    logger.logNow("logBuffer output available\n");
    }

void loop()
    {
    theMainController.run();											// in an endless loop, run the mainController
    }

void uart0_status_isr(void)
    {
    theMainController.serialInterrupt();								// handle Serial event interrupts
    }

void pit1_isr()
    {
    theMainController.pit1Interrupt();									// handle timer expire interrupts
    PIT_TFLG1 = 0x1;													// clear timer interrupt flag
    }

void pit2_isr()
    {
    theMainController.pit2Interrupt();									// handle timer expire interrupts
    PIT_TFLG2 = 0x1;													// clear timer interrupt flag
    }
