nadenken over hoe we de POSITIE gaan bijhouden ?
* niet constant van steps nr mm omrekenen
* positie in steps moeten we zeker kennen
* positie in mm wordt berekend in algorithme om te beslissen over step zetten
misschien is het beter om 2 aparte variabelen te hebben
currentPosition in steps
nextPosition in mm

how is sampleTime.motionTStop initialized ? it seems to happen when sequencing motions, but what about the first motion added tp an empty motionBuffer ?
sampleTime.motionDuration has the same problem

there are two options :
either let sampleTime have it's own copy of tStop and duration
or let sampleTime have a reference to the current motion (what if none is active) and get the info from there. This creates a dependency.

Problem is that after any optimization, this tStop may change..

options : 
* we could do it when we append the motion
* we could do it when we start / resume motionControl



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
1. output defaults when stopped (motionBuffer empty or not) : OK
2. output steps when running and motionItems in the buffer
3. proceed to next motionItem after completing the current one
4. go to Stopped after completing last motionItem
5. go to Stopped after passing tStop
6. time should proceed only when not stopped = running or stopping
7. stepsignals should always proceed : OK

More exotic conditions : 
1. output defaults when empty buffer : empty buffer and motionState == running should not occur, as at the end of the last motion, it returns the state to stopped
# Think about what to do with sampleTime after all motions are complete
    * reset to zero ?
    * I think we need to set t to the end of the motion and check if we need to take a last step


# think about timerReload expecting n-1 for a period of n clocks


