// ========================================================================================================
// File: test_broadcaster_stress_endurance.cpp
// Purpose: Extreme stress testing of the telemetry broadcaster using Dependency Injection.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>

#include "streaming/BLEManager.h"
#include "broadcast/GattConfigurator.hpp"
#include "broadcast/DataBroadcaster.hpp"
#include "broadcast/BleConnection.hpp"

// Instantiate the injected dependency chain explicitly (No Singletons)
BleConnection bleConn;
GattConfigurator bleGatt;
DataBroadcaster bleBroadcaster(bleGatt, bleConn);
BLEManager bleManager(bleConn, bleGatt, bleBroadcaster);

void setUp(void) {}
void tearDown(void) {}

void test_ble_initialization(void) {
    bool initSuccess = bleManager.begin();
    TEST_ASSERT_TRUE_MESSAGE(initSuccess, "BLE Manager failed to initialize. Check hardware radio module.");
}

void test_broadcaster_stress_endurance(void) {
    // Attempt 10,000 rapid characteristic writes, simulating heavy activity
    for (uint32_t i = 1; i <= 10000; i++) {
        bleBroadcaster.updateCounters(i, 0, 0);
        
        // Force the BLE stack to process the queued internal events
        bleManager.poll();
    }

    uint32_t finalSteps = bleGatt.getStepsChar().value().toInt();

    // Verify 100% data integrity post-stress
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(10000, finalSteps, "BLE Broadcaster dropped packets during stress execution.");
}

void setup() {
    delay(2000); 
    UNITY_BEGIN();
    
    RUN_TEST(test_ble_initialization);
    RUN_TEST(test_broadcaster_stress_endurance); 
    
    UNITY_END();
}

void loop() {}