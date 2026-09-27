#ifndef JUMP_MOTION_HPP
#define JUMP_MOTION_HPP

#include "detectors/BaseMotion.hpp"
#include "params/AlgoParams.hpp"

enum class JumpFsmState { NORMAL, FLIGHT };

class JumpMotion : public BaseMotion {
public:
    JumpMotion() : BaseMotion(MotionState::JUMP), jumpState_(JumpFsmState::NORMAL),
                   flightStartMs_(0), consecutiveFlightExitSamples_(0) {}

protected:
    bool preConditionMet(const MotionFeatures& features, const SlidingWindow& window) override {
        if (jumpState_ == JumpFsmState::NORMAL) {
            if (features.rawMag < AlgoParams::FLIGHT_ENTER_G) {
                jumpState_ = JumpFsmState::FLIGHT;
                flightStartMs_ = features.timestampMs;
                consecutiveFlightExitSamples_ = 0;
            }
        } else if (jumpState_ == JumpFsmState::FLIGHT) {
            if (features.timestampMs - flightStartMs_ > AlgoParams::FLIGHT_MAX_DURATION_MS) {
                jumpState_ = JumpFsmState::NORMAL;
                consecutiveFlightExitSamples_ = 0;
            }
        }
        return (jumpState_ == JumpFsmState::FLIGHT);
    }

    bool detectSpecificMotion(const MotionFeatures& features, const SlidingWindow& window) override {
        if (features.rawMag > AlgoParams::FLIGHT_EXIT_G) {
            consecutiveFlightExitSamples_++;
            
            if (consecutiveFlightExitSamples_ >= AlgoParams::JUMP_CONSECUTIVE_FRAMES) {
                uint32_t flightDuration = features.timestampMs - flightStartMs_;
                jumpState_ = JumpFsmState::NORMAL;

                if (flightDuration >= AlgoParams::FLIGHT_MIN_DURATION_MS && flightDuration <= AlgoParams::FLIGHT_MAX_DURATION_MS) {
                    return true;
                }
            }
        } else {
            consecutiveFlightExitSamples_ = 0;
        }
        return false;
    }

private:
    JumpFsmState jumpState_;
    uint32_t flightStartMs_;
    uint32_t consecutiveFlightExitSamples_;
};

#endif // JUMP_MOTION_HPP