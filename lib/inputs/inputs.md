How do digital inputs (eg. limitswitches and buttons) work ?

1. a single instance of the class 'inputs' initializes the GPIO on the HW
2. the class 'inputs' defines how many inputs there are
3. inputs.sample() reads all inputs from pins into memory
4. inputs.get(i) reads undebounced value of i-th input pin

Then this pin needs to be debounced. This is done through a generic class debounceInput.
1. Create an array of debouncedInput objects
2. The index of the item in the array matches with the index of the hardware pin
3. In the constructor, pass the events to raise when pin goes low-to-high and high-to-low
    eg : debouncedInput(event::limitSwitchXMinClosed, event::limitSwitchXMinOpened),

In main.loop()
1. periodically sample the GPIO, eg every 10ms
2. the for all members of the debounceInputs, run debouncedInput[i].getEvent();
