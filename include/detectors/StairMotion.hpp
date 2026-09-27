#ifndef STAIR_MOTION_HPP
#define STAIR_MOTION_HPP

#include "detectors/BaseMotion.hpp"
#include "params/AlgoParams.hpp"
#include "utils/PeakDetector.hpp"
#include <cmath>
#include <algorithm>

enum class StairFsmState { FLAT_GAIT, STAIR_CLIMBING };

class StairMotion : public BaseMotion {
public:
    StairMotion() : BaseMotion(MotionState::STAIRS), stairState_(StairFsmState::FLAT_GAIT),
                    shortTimeEnergyBuffer_(0.0f), consecutiveStairFrames_(0), stepRefractoryEndMs_(0) {}

protected:
    bool preConditionMet(const MotionFeatures& features, const SlidingWindow& window) override {
        float frameEnergy = (features.smoothMag - window.getMean()) * (features.smoothMag - window.getMean());
        shortTimeEnergyBuffer_ = (0.8f * shortTimeEnergyBuffer_) + (0.2f * frameEnergy);

        if (shortTimeEnergyBuffer_ > AlgoParams::STAIR_ENERGY_THRESHOLD && std::fabs(features.pitchAngle) > AlgoParams::STAIR_PITCH_THRESHOLD) {
            if (stairState_ == StairFsmState::FLAT_GAIT) {
                consecutiveStairFrames_++;
                if (consecutiveStairFrames_ >= AlgoParams::STAIR_CONSECUTIVE_FRAMES) {
                    stairState_ = StairFsmState::STAIR_CLIMBING;
                    consecutiveStairFrames_ = 0;
                }
            }
        } else {
            stairState_ = StairFsmState::FLAT_GAIT;
            consecutiveStairFrames_ = 0;
        }

        return (stairState_ == StairFsmState::STAIR_CLIMBING) && (features.timestampMs >= stepRefractoryEndMs_);
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
    StairFsmState stairState_;
    float shortTimeEnergyBuffer_;
    uint8_t consecutiveStairFrames_;
    uint32_t stepRefractoryEndMs_;
    PeakDetector peakDetector_;
};

#endif