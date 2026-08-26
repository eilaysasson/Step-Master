#include "hal/SampleTimer.h"

#include "BuildFlags.h"

#include <NRF52_MBED_TimerInterrupt.h>

volatile bool g_sampleDue = false;
volatile uint32_t g_expectedTicks = 0;

#if ENABLE_SAMPLE_DROP_COUNTER
volatile uint32_t g_sampleDrops = 0;
#else
volatile uint32_t g_sampleDrops = 0;
#endif

namespace
{
NRF52_MBED_Timer sampleTimer(NRF_TIMER_3);

void onSampleTick()
{
    // If a previous sample was still pending, count it as a drop.
    if (g_sampleDue)
    {
#if ENABLE_SAMPLE_DROP_COUNTER
        ++g_sampleDrops;
#endif
    }

    // Signal to the main loop that a new sample is ready to be read.
    g_sampleDue = true;

    // Track how many timer ticks have occurred for diagnostics.
    ++g_expectedTicks;
}
} // namespace

bool SampleTimer::begin(uint32_t intervalUs)
{
    if (started_)
    {
        return true;
    }

    g_sampleDue = false;
    g_expectedTicks = 0;
    g_sampleDrops = 0;

    // Attach the hardware timer and start periodic interrupts.
    started_ = sampleTimer.attachInterruptInterval(intervalUs, onSampleTick);
    return started_;
}

void SampleTimer::end()
{
    if (!started_)
    {
        return;
    }

    sampleTimer.detachInterrupt();
    started_ = false;
}
