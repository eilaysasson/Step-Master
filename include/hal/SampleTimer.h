#ifndef SAMPLE_TIMER_H
#define SAMPLE_TIMER_H

#include <cstdint>

/// Global flag set when the next sample should be read.
extern volatile bool g_sampleDue;

/// Count of timer ticks expected since startup or reset.
extern volatile uint32_t g_expectedTicks;

/// Count of sample drops detected when the previous sample was not consumed.
extern volatile uint32_t g_sampleDrops;

/// Hardware timer wrapper for generating periodic sample interrupts.
class SampleTimer
{
public:
    /// Start the timer with the requested interval in microseconds.
    bool begin(uint32_t intervalUs);

    /// Stop the timer and clear the started state.
    void end();

private:
    bool started_ = false;
};

#endif
