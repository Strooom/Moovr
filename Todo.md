1. Introduce namespaces :
*   namespace gcode {}
*   namespace inputs {}
*   namespace outputs {}
*   namespace motion {}
*   namespace hostinterface {}

2. Split timers (sampling input and writing output) into two separate objects.
*   output timer needs to go with HW implementation of stepper outputs
*   input sample timer could even be omitted and done with a SW timer.. as it is not time-critical and then it does no longer pose thread safety issues..




UART Tests
1. Send message, and receive it back through the loopback
2. Check before and after messageCount
3. Test underflow, overflow, etc..
4. Test for different CPUs : Teensy 3.5, 3.6, 4.0

TODOs

1. Check public, protected, private for all properties and methods
2. Check if parameters can be made const
3. Get rid of char and replace with uint8_t
4. Add critical sections for access to rx/txBuffer !! sei(); cli();
5. Add a lastError and test that it triggers when it should and not if it shouldn't