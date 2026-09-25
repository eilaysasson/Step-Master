#ifndef BASE_MOTION_HPP
#define BASE_MOTION_HPP

#include <cstdint>
#include "params/MotionState.h"
#include "sensors/MotionFeatures.h"
#include "utils/SlidingWindow.hpp"

class BaseMotion {
public:
    BaseMotion(MotionState type) : type_(type), count_(0) {}
    virtual ~BaseMotion() = default;

    MotionState evaluate(const MotionFeatures& features, const SlidingWindow& window) {
        if (!preConditionMet(features, window)) {
            return MotionState::IDLE;
        }

        if (detectSpecificMotion(features, window)) {
            count_++;
            return type_;
        }
        return MotionState::IDLE;
    }

    uint32_t getCount() const { return count_; }
    MotionState getType() const { return type_; }
    void resetCount() { count_ = 0; }

protected:
    virtual bool preConditionMet(const MotionFeatures& features, const SlidingWindow& window) = 0;
    virtual bool detectSpecificMotion(const MotionFeatures& features, const SlidingWindow& window) = 0;

private:
    MotionState type_;
    uint32_t count_;
};

#endif // BASE_MOTION_HPP