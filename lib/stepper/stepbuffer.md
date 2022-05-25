Controlling the stepper motors means outputting the 'step' and 'direction' signals with the correct timing.
In order to have the timing really accurate, the best way is to do this based in hardware timers with interrupts :
    * a hardware timer is loaded with the 'time' until the next signal-change
    * when the timer expires, the next signals are written to output

In order to set the timer with a timeout, you write its RELOAD registers.
On the next timeout of the timer, it will then load this RELOAD value into the timer and start ticking down

This means the time before outputting motor signals needs to be written to RELOAD two (interrupt) events earlier :
* time T=-Z : output stepsignals Y - set RELOAD to A - timer loaded with Z
* time T=0 :  output stepsignals Z - set RELOAD to B - timer loaded with A
* time T=A :  output stepsignals A - set RELOAD to C - timer loaded with B

So a step is essentially
* motor signals
* the time between previous motor signals and new motor signals

This mechanism allows to fill the stepBuffer with steps and have them output with accurate timing and with minimal CPU intervention.
Filling the buffer is by calculating new steps. Calculating steps happens in a background thread.
Emptying the buffer is by hardware timer interrupts.

Because the background thread may be busy doing other calculations, you need sufficient steps in the stepBuffer, so it does not run empty before the background thread can fill it again.
On the other hand, all steps in the buffer are 'final' and can't be changed anymore. So we should have not too many steps in this buffer.
When a certain event requires a change of the motion of the machine (eg feedhold, change of feedrate), it can only have effect after the steps already in the stepBuffer have been done.

In order to keep the buffer filled up to the ideal level, it keeps track of how much 'time' there is in the buffer, ie, the sum of all times between all output signals. This time can then be kept to eg. 10 ms, giving us always 10 ms to calculate new steps, while only delaying a reaction to machine inputs to max. 10 ms.

