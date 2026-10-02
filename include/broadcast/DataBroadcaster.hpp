// ========================================================================================================
// File: DataBroadcaster.hpp
// Purpose: Handles checking for state changes and dispatching updates to the GATT configurator.
// ========================================================================================================

#ifndef DATA_BROADCASTER_HPP
#define DATA_BROADCASTER_HPP

#include <cstdint>
#include <Arduino.h>
#include "broadcast/GattConfigurator.hpp"
#include "broadcast/BleConnection.hpp"
#include "params/MotionState.h"

class DataBroadcaster {
public:
    // Dependencies injected via constructor
    DataBroadcaster(GattConfigurator& gatt, BleConnection& ble) 
        : gatt_(gatt), ble_(ble), lastSteps_(0), lastJumps_(0), lastStairs_(0), lastState_(MotionState::IDLE) {}

    void updateCounters(uint32_t steps, uint32_t jumps, uint32_t stairs) {
        if (steps != lastSteps_) {
            gatt_.getStepsChar().writeValue(String(steps));
            lastSteps_ = steps;
            ble_.poll();
        }
        
        // This logic was missing/skipped in your current file
        if (jumps != lastJumps_) {
            gatt_.getJumpsChar().writeValue(String(jumps));
            lastJumps_ = jumps;
            ble_.poll();
        }
        
        if (stairs != lastStairs_) {
            gatt_.getStairsChar().writeValue(String(stairs));
            lastStairs_ = stairs;
            ble_.poll();
        }
    }

    void updateState(MotionState state) {
        if (state != lastState_) {
            String stateStr = "IDLE";
            if (state == MotionState::STEP) stateStr = "STEP";
            else if (state == MotionState::JUMP) stateStr = "JUMP";
            else if (state == MotionState::STAIRS) stateStr = "STAIRS";
            else if (state == MotionState::NOISE) stateStr = "NOISE";

            gatt_.getStateChar().writeValue(stateStr);
            lastState_ = state;
            ble_.poll();
        }
    }

private:
    GattConfigurator& gatt_;
    BleConnection& ble_;
    uint32_t lastSteps_;
    uint32_t lastJumps_;
    uint32_t lastStairs_;
    MotionState lastState_;
};

#endif // DATA_BROADCASTER_HPP