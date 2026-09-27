#ifndef STEP_MOTION_HPP
#define STEP_MOTION_HPP

#include "detectors/BaseMotion.hpp"
#include "params/AlgoParams.hpp"
#include "utils/PeakDetector.hpp"
#include <algorithm>

class StepMotion : public BaseMotion {
public:
    StepMotion() : BaseMotion(MotionState::STEP), stepRefractoryEndMs_(0) {}

protected:
    bool preConditionMet(const MotionFeatures& features, const SlidingWindow& window) override {
        return features.timestampMs >= stepRefractoryEndMs_;
    }

    bool detectSpecificMotion(const MotionFeatures& features, const SlidingWindow& window) override {
        float dynamicThreshold = window.getMean() + (AlgoParams::STEP_K_FACTOR * window.getStdDev());
        float clampedThreshold = std::max(AlgoParams::STEP_THRESHOLD_MIN_G, std::min(dynamicThreshold, AlgoParams::STEP_THRESHOLD_MAX_G));

        float peakValue = 0.0f;
        if (peakDetector_.update(features.smoothMag, peakValue) && peakValue > clampedThreshold) {
            stepRefractoryEndMs_ = features.timestampMs + AlgoParams::STEP_REFRACTORY_MS;
            return true;
        }
        return false;
    }

private:
    uint32_t stepRefractoryEndMs_;
    PeakDetector peakDetector_;
};

#endif // STEP_MOTION_HPP