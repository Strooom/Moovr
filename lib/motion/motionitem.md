# How does a motion calculates it's position in steps ?

1. it advances time with certain intervals in an object sampleTime
2. then it calculates the distance travelled over the trajectory using sTravelled = motion.s(sampleTime)
3. then it calculates the position at the trajectory using motion.position(sTravelled)
4. this position is then converted from mm to steps
5. then it determines if this new position requires to take a step from the current position


