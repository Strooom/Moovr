States : 
1. Stopped
    calcNextStepperMotorSignals should be outputting all zero signals, at the default rate
    * with empty motionBuffer
    * with non-empty motionBuffer, but after issueing a stop/feedhold

2. Running / Stopping
    * standard case : in the middle of a motionItem
    * roll over into next motionItem
    * complete last motionItem
    * beyond tStop

What should it do ?
1. output defaults when empty buffer
2. output defaults when non-empty buffer but stopped
3. output steps when running and motionItems in the buffer
4. proceed to next motionItem after completing the current one
5. go to Stopped after completing last motionItem
6. go to Stopped after passing tStop
7. time should proceed only when not stopped = running or stopping
8. stepsignals should always proceed

# Think about what to do with sampleTime after all motions are complete
    * reset to zero ?


# think about timerReload expecting n-1 for a period of n clocks


