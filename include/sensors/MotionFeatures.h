#ifndef MOTION_FEATURES_H
#define MOTION_FEATURES_H

#include <cstdint>

struct MotionFeatures {
    float rawMag;
    float smoothMag;
    float pitchAngle;
    uint32_t timestampMs;
};

#endif // MOTION_FEATURES_H