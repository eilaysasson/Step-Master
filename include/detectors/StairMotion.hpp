#ifndef STAIR_MOTION_HPP
#define STAIR_MOTION_HPP

#include "detectors/BaseMotion.hpp"
#include "params/AlgoParams.hpp"
#include "utils/PeakDetector.hpp"
#include <cmath>
#include <algorithm>

class StairMotion : public BaseMotion {
public:
    StairMotion() : BaseMotion(MotionState::STAIRS), stepRefractoryEndMs_(0) {}

protected:
    bool preConditionMet(const MotionFeatures& features, const SlidingWindow& window) override {
        return true;
    }

    bool detectSpecificMotion(const MotionFeatures& features, const SlidingWindow& window) override {
        float peakValue = 0.0f;
        bool isPeak = peakDetector_.update(features.smoothMag, peakValue);

        if (isPeak) {
            if (features.timestampMs < stepRefractoryEndMs_) return false;

            float dynamicThreshold = window.getMean() + (AlgoParams::STEP_K_FACTOR * window.getStdDev());
            float clampedThreshold = std::max(AlgoParams::STEP_THRESHOLD_MIN_G, std::min(dynamicThreshold, AlgoParams::STEP_THRESHOLD_MAX_G));

            // Stairs represent a distinct lifting of the leg without hitting full knee-to-chest height.
            float absPitch = std::fabs(features.pitchAngle);
            if (peakValue > clampedThreshold && 
                absPitch >= AlgoParams::PITCH_THRESHOLD_STAIRS && 
                absPitch < AlgoParams::PITCH_THRESHOLD_JUMP) {
                
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

#endif // STAIR_MOTION_HPP