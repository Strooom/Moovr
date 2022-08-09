API : 
    sampleTime(const float theMinStepPulseWidth);        // constructor
    void next();                                         // calculate the position of the next sampling point
    void reset();                                        //
    bool isBeyondEndOfMotion() const;                    //
    bool isBeyondStop() const;                           //
    void nextMotion();                                   //
    float timeInMotion{0.0F};                            // [s]
    void setMotionTStop(float someStopTime);
    void setMotionDuration(float someMotionDuration);


1. Initialization / Re-initialization
2. increment sample time
3. test isBeyonds..
4. increment to next motion

