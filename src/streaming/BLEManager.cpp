// ========================================================================================================
// File: BLEManager.cpp
// Purpose: Implementation of the BLEManager Facade orchestrating the injected components.
// ========================================================================================================

#include "streaming/BLEManager.h"
#include <Arduino.h>

BLEManager::BLEManager(BleConnection& conn, GattConfigurator& gatt, DataBroadcaster& broadcaster)
    : conn_(conn), gatt_(gatt), broadcaster_(broadcaster) {}

bool BLEManager::begin() {
    if (!conn_.begin()) {
        Serial.println("[ERROR] BLE radio initialization failed!");
        return false;
    }

    gatt_.setupServices();
    conn_.startAdvertising();

    Serial.println("[INFO] BLE Facade initialized successfully. Advertising...");
    return true;
}

void BLEManager::poll() {
    conn_.poll();
}

void BLEManager::updateTelemetry(MotionState state, uint32_t steps, uint32_t jumps, uint32_t stairs) {
    if (conn_.isConnected()) {
        broadcaster_.updateState(state);
        broadcaster_.updateCounters(steps, jumps, stairs);
    }
}