Nxt Tasks
=========
* moved strategy and overrides into motionCtrl, provide getter and use this getter everywhere else these are needed..  
* build a minimal main application, binding together all subsystems
* expose some privates to unitTesting, keep others fully private

#ifndef unitTesting
  private:        // private in normal operation, accessible during unitTesting
#else
  public:
#endif
int var1{0};
int func1();


  private: // always private
int var2{0};
int func2();


Solve how to do different builds for different HW targets - taking as a test-case to build a 3-axis CNC and a 2-axis Plotter


TODOs

1. Check public, protected, private for all properties and methods
2. Check if parameters can be made const
3. Get rid of char and replace with uint8_t uint32_t
4. Add critical sections for access to rx/txBuffer !! sei(); cli();
5. Add a lastError and test that it triggers when it should and not if it shouldn't










=======================================


2. Split timers (sampling input and writing output) into two separate objects.
*   output timer needs to go with HW implementation of stepper outputs
*   input sample timer could even be omitted and done with a SW timer.. as it is not time-critical and then it does no longer pose thread safety issues..



UART Tests
1. Send message, and receive it back through the loopback
2. Check before and after messageCount
3. Test underflow, overflow, etc..
4. Test for different CPUs : Teensy 3.5, 3.6, 4.0
