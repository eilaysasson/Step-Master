#include "hal/SampleTimer.h"
#include <Arduino.h>
#include <NRF52_MBED_TimerInterrupt.h>

volatile bool g_sampleDue = false;
volatile uint32_t g_expectedTicks = 0;
volatile uint32_t g_sampleDrops = 0;

namespace {
    NRF52_MBED_Timer sampleTimer(NRF_TIMER_3);

    void onSampleTick() {
        if (g_sampleDue) {
            g_sampleDrops++;
        }
        g_sampleDue = true;
        g_expectedTicks++;
    }
}

bool SampleTimer::begin(uint32_t intervalUs) {
    if (started_) return true;
    
    g_sampleDue = false;
    g_expectedTicks = 0;
    g_sampleDrops = 0;

    started_ = sampleTimer.attachInterruptInterval(intervalUs, onSampleTick);
    return started_;
}

void SampleTimer::end() {
    if (!started_) return;
    sampleTimer.detachInterrupt();
    started_ = false;
}

bool SampleTimer::consumeSample() {
    // Atomic read-and-clear to prevent race conditions with the hardware timer ISR.
    noInterrupts();
    if (!g_sampleDue) {
        interrupts();
        return false;
    }
    g_sampleDue = false;
    interrupts();
    return true;
}