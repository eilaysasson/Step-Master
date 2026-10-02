#ifndef BLE_CONNECTION_HPP
#define BLE_CONNECTION_HPP

#include <ArduinoBLE.h>
#include "config/BLEConfig.hpp"

class BleConnection {
public:
    BleConnection() = default;

    bool begin() {
        if (!BLE.begin()) return false;
        BLE.setLocalName(BLEConfig::DEVICE_NAME);
        return true;
    }

    void startAdvertising() { BLE.advertise(); }
    void poll() { BLE.poll(); }
    bool isConnected() const {
        BLEDevice central = BLE.central();
        return central && central.connected();
    }
};

#endif // BLE_CONNECTION_HPP