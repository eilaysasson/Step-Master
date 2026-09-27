// ========================================================================================================
// File: BleConnection.hpp
// Purpose: Singleton class managing the physical BLE radio state, connections, and advertising.
// ========================================================================================================

#ifndef BLE_CONNECTION_HPP
#define BLE_CONNECTION_HPP

#include <ArduinoBLE.h>
#include "config/BLEConfig.hpp"

class BleConnection {
public:
    // ----------------------------------------------------------------------------------------------------
    // Method: getInstance
    // Purpose: Returns the single, globally accessible instance of the BleConnection class.
    // ----------------------------------------------------------------------------------------------------
    static BleConnection& getInstance() {
        static BleConnection instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to enforce Singleton pattern
    BleConnection(const BleConnection&) = delete;
    BleConnection& operator=(const BleConnection&) = delete;

    // ----------------------------------------------------------------------------------------------------
    // Method: begin
    // Purpose: Initializes the physical BLE module and sets the local device name.
    // ----------------------------------------------------------------------------------------------------
    bool begin() {
        if (!BLE.begin()) {
            return false;
        }
        BLE.setLocalName(BLEConfig::DEVICE_NAME);
        return true;
    }

    // ----------------------------------------------------------------------------------------------------
    // Method: startAdvertising
    // Purpose: Commands the BLE radio to begin broadcasting its presence to central devices.
    // ----------------------------------------------------------------------------------------------------
    void startAdvertising() {
        BLE.advertise();
    }

    // ----------------------------------------------------------------------------------------------------
    // Method: poll
    // Purpose: Polls the BLE hardware for new connection events and data requests.
    // ----------------------------------------------------------------------------------------------------
    void poll() {
        BLE.poll();
    }

    // ----------------------------------------------------------------------------------------------------
    // Method: isConnected
    // Purpose: Checks if a central device (like a smartphone) is currently connected to the peripheral.
    // ----------------------------------------------------------------------------------------------------
    bool isConnected() const {
        BLEDevice central = BLE.central();
        return central && central.connected();
    }

private:
    // Private constructor to prevent external instantiation
    BleConnection() = default;
};

#endif // BLE_CONNECTION_HPP