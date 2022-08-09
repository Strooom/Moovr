How does Moovr take care of thread-safety ?
* when data can be accessed from two different 'threads' this is unsafe. Data could be in the middle of being modified by one thread, while being read by another
* a typical case are interrupt handlers : data is being modified by an interrupt handler in the middle of being read by the main program

In Moovr we have the following interrupt handlers
* PIT : a timer to write stepsignals to motor outputs
* UART : interface to the host

Whenever there is a call to one of these two systems, the interrupts to it must be temporarily be suspended, so no interrupts can occur in that section
This suspension must of course be as short as possible

As in Moovr all components are joined together in the main program, this main.cpp is the only place where this should occur.