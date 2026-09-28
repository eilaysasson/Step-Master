#ifndef STEP_MOTION_HPP
#define STEP_MOTION_HPP

#include "detectors/BaseMotion.hpp"
#include "params/AlgoParams.hpp"
#include "utils/PeakDetector.hpp"
#include <algorithm>
#include <cmath>

class StepMotion : public BaseMotion {
public:
    StepMotion() : BaseMotion(MotionState::STEP), stepRefractoryEndMs_(0) {}

protected:
    bool preConditionMet(const MotionFeatures& features, const SlidingWindow& window) override {
        // Must return true every frame so the PeakDetector history isn't starved.
        return true;
    }

    bool detectSpecificMotion(const MotionFeatures& features, const SlidingWindow& window) override {
        float peakValue = 0.0f;
        // Peak detector MUST be fed every single frame to maintain accurate n-1/n-2 history.
        bool isPeak = peakDetector_.update(features.smoothMag, peakValue);

        if (isPeak) {
            // 1. Check debounce refractory period
            if (features.timestampMs < stepRefractoryEndMs_) return false;

            // 2. Calculate adaptive thresholds
            float dynamicThreshold = window.getMean() + (AlgoParams::STEP_K_FACTOR * window.getStdDev());
            float clampedThreshold = std::max(AlgoParams::STEP_THRESHOLD_MIN_G, std::min(dynamicThreshold, AlgoParams::STEP_THRESHOLD_MAX_G));

            // 3. Pitch-based isolation (Steps, light hops, running = low pitch)
            float absPitch = std::fabs(features.pitchAngle);
            if (peakValue > clampedThreshold && absPitch < AlgoParams::PITCH_THRESHOLD_STAIRS) {
                stepRefractoryEndMs_ = features.timestampMs + AlgoParams::STEP_REFRACTORY_MS;
                return true;
            }
        }
        return false;
    }

private:
    uint32_t stepRefractoryEndMs_;
    PeakDetector peakDetector_;
};

#endif // STEP_MOTION_HPP