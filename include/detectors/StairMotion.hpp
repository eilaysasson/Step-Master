// ========================================================================================================
// File: StairMotion.hpp
// Purpose: Concrete implementation of BaseMotion for detecting vertical stair climbing and descending.
// Features: Isolates stairs by bounding the pitch angle between flat walking and extreme jumping.
// ========================================================================================================

#ifndef STAIR_MOTION_HPP
#define STAIR_MOTION_HPP

#include "detectors/BaseMotion.hpp"
#include <algorithm>
#include <cmath>

class StairMotion : public BaseMotion {
public:
    StairMotion() : BaseMotion(MotionState::STAIRS) {}

protected:
    bool detectSpecificMotion(const MotionFeatures& features, const AlgoConfig& config) override {
        // A valid peak must exist in the current frame to evaluate motion
        if (!features.isPeak) return false;

        // Calculate the dynamic threshold based on the centralized window statistics
        float dynamicThreshold = features.windowMean + (config.stepKFactor * features.windowStdDev);
        float clampedThreshold = std::max(config.stepThresholdMinG, std::min(dynamicThreshold, config.stepThresholdMaxG));
        
        float absPitch = std::fabs(features.pitchAngle);

        // Stairs require a pitch angle strictly bounded between normal walking and extreme jumping limits
        if (features.peakValue > clampedThreshold && 
            absPitch >= config.pitchThresholdStairs && 
            absPitch < config.pitchThresholdJump) {
            return true;
        }
        
        return false;
    }
};

#endif // STAIR_MOTION_HPP