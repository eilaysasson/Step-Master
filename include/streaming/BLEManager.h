// ========================================================================================================
// File: BLEManager.h
// Purpose: Facade class providing a simplified interface to the injected BLE subsystems.
// ========================================================================================================

#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <cstdint>
#include "params/MotionState.h"
#include "broadcast/BleConnection.hpp"
#include "broadcast/GattConfigurator.hpp"
#include "broadcast/DataBroadcaster.hpp"

class BLEManager {
public:
    BLEManager(BleConnection& conn, GattConfigurator& gatt, DataBroadcaster& broadcaster);
    
    bool begin();
    void poll();
    void updateTelemetry(MotionState state, uint32_t steps, uint32_t jumps, uint32_t stairs);

private:
    BleConnection& conn_;
    GattConfigurator& gatt_;
    DataBroadcaster& broadcaster_;
};

#endif // BLE_MANAGER_H