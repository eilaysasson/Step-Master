#ifndef BASE_MOTION_HPP
#define BASE_MOTION_HPP

#include <cstdint>
#include "params/MotionState.h"
#include "sensors/MotionFeatures.h"
#include "utils/TimeGuard.hpp"
#include "config/AlgoConfig.hpp"

class BaseMotion {
public:
    BaseMotion(MotionState type) : type_(type), count_(0) {}
    virtual ~BaseMotion() = default;

    MotionState evaluate(const MotionFeatures& features, const AlgoConfig& config) {
        // Shared debounce handling eliminates duplicated logic across all child detectors.
        if (!timeGuard_.isReady(features.timestampMs)) {
            return MotionState::IDLE;
        }

        if (detectSpecificMotion(features, config)) {
            timeGuard_.setCooldown(features.timestampMs, config.stepRefractoryMs);
            count_++;
            return type_;
        }
        
        return MotionState::IDLE;
    }

    uint32_t getCount() const { return count_; }
    MotionState getType() const { return type_; }
    void resetCount() { count_ = 0; }

protected:
    // preConditionMet() is removed entirely. Detectors only implement physical logic.
    virtual bool detectSpecificMotion(const MotionFeatures& features, const AlgoConfig& config) = 0;

private:
    TimeGuard timeGuard_;
    MotionState type_;
    uint32_t count_;
};

#endif // BASE_MOTION_HPP