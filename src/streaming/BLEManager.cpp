// ========================================================================================================
// File: BLEManager.cpp
// Purpose: Implementation of the BLEManager Facade orchestrating the underlying Singleton components.
// ========================================================================================================

#include "streaming/BLEManager.h"
#include "broadcast/BleConnection.hpp"
#include "broadcast/GattConfigurator.hpp"
#include "broadcast/DataBroadcaster.hpp"
#include <Arduino.h>

bool BLEManager::begin() {
    if (!BleConnection::getInstance().begin()) {
        Serial.println("[ERROR] BLE radio initialization failed!");
        return false;
    }

    GattConfigurator::getInstance().setupServices();
    BleConnection::getInstance().startAdvertising();

    Serial.println("[INFO] BLE Facade initialized successfully. Advertising...");
    return true;
}

void BLEManager::poll() {
    BleConnection::getInstance().poll();
}

void BLEManager::updateTelemetry(MotionState state, uint32_t steps, uint32_t jumps, uint32_t stairs) {
    if (BleConnection::getInstance().isConnected()) {
        // DataBroadcaster now internally handles state-tracking, subscription validation, and stack polling
        DataBroadcaster::getInstance().updateState(state);
        DataBroadcaster::getInstance().updateCounters(steps, jumps, stairs);
    }
}