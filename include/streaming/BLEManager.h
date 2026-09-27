// ========================================================================================================
// File: BLEManager.h
// Purpose: Facade class providing a simplified interface to the BLE subsystems (Connection, GATT, Broadcast).
// ========================================================================================================

#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <cstdint>
#include "params/MotionState.h"

class BLEManager {
public:
    // ----------------------------------------------------------------------------------------------------
    // Method: begin
    // Purpose: Orchestrates the initialization of the BLE hardware, GATT configuration, and starts advertising.
    // ----------------------------------------------------------------------------------------------------
    bool begin();

    // ----------------------------------------------------------------------------------------------------
    // Method: poll
    // Purpose: Delegates polling execution to the underlying BLE connection manager to handle radio events.
    // ----------------------------------------------------------------------------------------------------
    void poll();

    // ----------------------------------------------------------------------------------------------------
    // Method: updateTelemetry
    // Purpose: Transmits motion state and updated counter values to connected central devices if a link exists.
    // ----------------------------------------------------------------------------------------------------
    void updateTelemetry(MotionState state, uint32_t steps, uint32_t jumps, uint32_t stairs);
};

#endif // BLE_MANAGER_H