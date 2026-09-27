#ifndef SAMPLE_TIMER_H
#define SAMPLE_TIMER_H

#include <cstdint>
#include "params/BuildFlags.h"

extern volatile bool g_sampleDue;
extern volatile uint32_t g_expectedTicks;
extern volatile uint32_t g_sampleDrops;

class SampleTimer {
public:
    bool begin(uint32_t intervalUs);
    void end();
    
    // Safely clears the ISR flag and returns true if a sample is due.
    bool consumeSample();

private:
    bool started_ = false;
};

#endif // SAMPLE_TIMER_H