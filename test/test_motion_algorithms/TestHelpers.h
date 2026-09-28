// ========================================================================================================
// File: TestHelpers.h
// Purpose: Provides robust deterministic signal injection to perfectly test the Pitch-Based FSM logic.
// ========================================================================================================

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "algorithem/MotionClassifier.h"

// ----------------------------------------------------------------------------------------------------
// Method: stabilizeWindow
// Purpose: Feeds resting data to initialize the Sliding Window mean and standard deviation.
// ----------------------------------------------------------------------------------------------------
inline void stabilizeWindow(MotionClassifier& classifier, uint32_t& clockMs) {
    MotionData idleData = {0};
    idleData.accelZ = 1.0f;
    for (int i = 0; i < 100; i++) {
        idleData.timestampMs = clockMs;
        classifier.update(idleData);
        clockMs += 10;
    }
}

// ----------------------------------------------------------------------------------------------------
// Method: injectKinematicPeak
// Purpose: Mathematically simulates a human leg swing followed by a ground impact (Peak).
//          It gives the Complementary Filter time to settle on the correct Pitch angle before impact.
// ----------------------------------------------------------------------------------------------------
inline void injectKinematicPeak(MotionClassifier& classifier, uint32_t& clockMs, float baseAx, float baseAz, float peakMultiplier = 2.0f) {
    MotionData data = {0};
    
    // 1. Swing Phase (100 frames = 1 sec): Settle the Complementary Filter (gyro/accel) to target pitch
    data.accelX = baseAx;
    data.accelZ = baseAz;
    for (int i = 0; i < 100; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }

    // 2. Impact Phase (2 frames): Generate a high-G spike to trigger the PeakDetector
    data.accelX = baseAx * peakMultiplier;
    data.accelZ = baseAz * peakMultiplier;
    for (int i = 0; i < 2; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }

    // 3. Recovery Phase (5 frames): Drop back down to confirm the falling edge of the peak
    data.accelX = baseAx;
    data.accelZ = baseAz;
    for (int i = 0; i < 5; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }

    // 4. Debounce Cooldown (30 frames): Wait out the 250ms refractory period for the next test
    data.accelX = 0.0f;
    data.accelZ = 1.0f;
    for (int i = 0; i < 30; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }
}

#endif // TEST_HELPERS_H