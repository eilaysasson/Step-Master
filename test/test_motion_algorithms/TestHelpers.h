#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "algorithem/MotionClassifier.h"

// Advances the classifier state to stabilize the sliding window calculations
inline void stabilizeWindow(MotionClassifier& classifier, uint32_t& clockMs) {
    MotionData idleData = {0};
    idleData.accelZ = 1.0f;
    for (int i = 0; i < 100; i++) {
        idleData.timestampMs = clockMs;
        classifier.update(idleData);
        clockMs += 10;
    }
}

#endif // TEST_HELPERS_H