Most of this can be tested without target HW.
However the final application of this stepBuffer is to output bit patters with a timing controlled by HW-timer. In order to validate the timings, an minimal application is needed and an oscilloscope to visualize the resulting signals and timings :
* respecting setup time for a motor direction change
* respecting minimal step pulse time
* respecting the time between to step events

API : 
    void initialize();
    void write(step aStep);
    step read();
    uint32_t getTimeInTicks() const;
    uint32_t getLevel() const;
    bool needsFilling() const;

1. Initialization / Re-initialization
2. Push / Pop items
* data integrity
* index wrapping
3. Overflow / Underflow
* error event logging
* recovery


