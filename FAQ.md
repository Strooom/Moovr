Q : On which microcontrollers can Moovr run ?
A : Moovr was designed to be portable towards modern MCU's with at least the following features:
 * HW support for floating point calculations, eg Arm Cortex M4F (F indicating floating point support)
 * at least one PIT : programmable interval timer.
 * 32bit architecture

Note : EPS32 does not have hardware floating point support and as such does not qualify


 Q : Why does Moovr requires a hardware floating point MCU ?
 A : In order to have an accurate and smooth movement of the machine, motion calculations are done in floating point math.
     In order to achieve high step rates, these calculations require HW accelereation.

Q : How many axis does Moovr support ?
A : In fact there is no real limit.

Q : What step rates does Moovr achieve ?
A : It depends on your hardware, but typically > 100K steps/second. Typically Moovr will not be the bottleneck.