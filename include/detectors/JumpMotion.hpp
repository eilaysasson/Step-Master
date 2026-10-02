// ========================================================================================================
// File: JumpMotion.hpp
// Purpose: Concrete implementation of BaseMotion specifically for detecting airborne jump events.
// Features: Uses dynamic thresholds and extreme pitch limits evaluated through the extracted feature struct.
// ========================================================================================================

#ifndef JUMP_MOTION_HPP
#define JUMP_MOTION_HPP

#include "detectors/BaseMotion.hpp"
#include <algorithm>
#include <cmath>

class JumpMotion : public BaseMotion {
public:
    JumpMotion() : BaseMotion(MotionState::JUMP) {}

protected:
    bool detectSpecificMotion(const MotionFeatures& features, const AlgoConfig& config) override {
        // A valid peak must exist in the current frame to evaluate motion
        if (!features.isPeak) return false;

        // Calculate the dynamic threshold based on the centralized window statistics
        float dynamicThreshold = features.windowMean + (config.stepKFactor * features.windowStdDev);
        float clampedThreshold = std::max(config.stepThresholdMinG, std::min(dynamicThreshold, config.stepThresholdMaxG));
        
        float absPitch = std::fabs(features.pitchAngle);

        // Jumps (e.g., knees-to-chest, high kicks) require extreme rotational pitch
        if (features.peakValue > clampedThreshold && absPitch >= config.pitchThresholdJump) {
            return true;
        }
        
        return false;
    }
};

#endif // JUMP_MOTION_HPP