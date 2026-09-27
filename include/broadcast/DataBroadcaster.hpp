// ========================================================================================================
// File: DataBroadcaster.hpp
// Purpose: Singleton class handling the transmission of telemetry data over the configured BLE characteristics.
// ========================================================================================================

#ifndef DATA_BROADCASTER_HPP
#define DATA_BROADCASTER_HPP

#include <cstdint>
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
        // Dedup: Only write and poll the BLE stack if the specific counter has actually changed.
        // ArduinoBLE handles the decision of whether to send a notification under the hood.
        if (steps != lastSteps_) {
            GattConfigurator::getInstance().getStepsChar().writeValue(steps);
            lastSteps_ = steps;
            BleConnection::getInstance().poll();
        }
        
        if (jumps != lastJumps_) {
            GattConfigurator::getInstance().getJumpsChar().writeValue(jumps);
            lastJumps_ = jumps;
            BleConnection::getInstance().poll();
        }
        
        if (stairs != lastStairs_) {
            GattConfigurator::getInstance().getStairsChar().writeValue(stairs);
            lastStairs_ = stairs;
            BleConnection::getInstance().poll();
        }
    }

    void updateState(MotionState state) {
        if (state != lastState_) {
            GattConfigurator::getInstance().getStateChar().writeValue(static_cast<uint8_t>(state));
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