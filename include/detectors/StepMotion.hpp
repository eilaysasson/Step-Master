#ifndef STEP_MOTION_HPP
#define STEP_MOTION_HPP

#include "detectors/BaseMotion.hpp"
#include <algorithm>
#include <cmath>

class StepMotion : public BaseMotion {
public:
    StepMotion() : BaseMotion(MotionState::STEP) {}

protected:
    bool detectSpecificMotion(const MotionFeatures& features, const AlgoConfig& config) override {
        if (!features.isPeak) return false;

        float dynamicThreshold = features.windowMean + (config.stepKFactor * features.windowStdDev);
        float clampedThreshold = std::max(config.stepThresholdMinG, std::min(dynamicThreshold, config.stepThresholdMaxG));
        
        float absPitch = std::fabs(features.pitchAngle);

        if (features.peakValue > clampedThreshold && absPitch < config.pitchThresholdStairs) {
            return true;
        }
        
        return false;
    }
};

#endif // STEP_MOTION_HPP