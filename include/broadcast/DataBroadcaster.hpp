#ifndef DATA_BROADCASTER_HPP
#define DATA_BROADCASTER_HPP

#include <cstdint>
#include <Arduino.h>
#include "GattConfigurator.hpp"
#include "BleConnection.hpp"
#include "params/MotionState.h"

class DataBroadcaster {
public:
    static DataBroadcaster& getInstance() {
        static DataBroadcaster instance;
        return instance;
    }

    DataBroadcaster(const DataBroadcaster&) = delete;
    DataBroadcaster& operator=(const DataBroadcaster&) = delete;

    void updateCounters(uint32_t steps, uint32_t jumps, uint32_t stairs) {
        // String conversion happens ONLY when a counter actually changes, ensuring top performance.
        if (steps != lastSteps_) {
            GattConfigurator::getInstance().getStepsChar().writeValue(String(steps));
            lastSteps_ = steps;
            BleConnection::getInstance().poll();
        }
        
        if (jumps != lastJumps_) {
            GattConfigurator::getInstance().getJumpsChar().writeValue(String(jumps));
            lastJumps_ = jumps;
            BleConnection::getInstance().poll();
        }
        
        if (stairs != lastStairs_) {
            GattConfigurator::getInstance().getStairsChar().writeValue(String(stairs));
            lastStairs_ = stairs;
            BleConnection::getInstance().poll();
        }
    }

    void updateState(MotionState state) {
        if (state != lastState_) {
            String stateStr = "IDLE";
            if (state == MotionState::STEP) stateStr = "STEP";
            else if (state == MotionState::JUMP) stateStr = "JUMP";
            else if (state == MotionState::STAIRS) stateStr = "STAIRS";
            else if (state == MotionState::NOISE) stateStr = "NOISE";

            GattConfigurator::getInstance().getStateChar().writeValue(stateStr);
            lastState_ = state;
            BleConnection::getInstance().poll();
        }
    }

private:
    DataBroadcaster() = default;

    uint32_t lastSteps_ = 0;
    uint32_t lastJumps_ = 0;
    uint32_t lastStairs_ = 0;
    MotionState lastState_ = MotionState::IDLE;
};

#endif // DATA_BROADCASTER_HPP