// ========================================================================================================
// File: MotionFeatures.h
// Purpose: A centralized data structure holding fully processed kinematic features for the current frame.
// ========================================================================================================

#ifndef MOTION_FEATURES_H
#define MOTION_FEATURES_H

#include <cstdint>

struct MotionFeatures {
    float rawMag;
    float smoothMag;
    float pitchAngle;
    uint32_t timestampMs;
    
    // Extracted from the shared SlidingWindow
    float windowMean;
    float windowStdDev;
    
    // Extracted from the shared PeakDetector
    bool isPeak;
    float peakValue;
};

#endif // MOTION_FEATURES_H