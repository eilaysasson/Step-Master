// ========================================================================================================
// File: test_ble_services.cpp
// Purpose: Validates the BLE Manager Facade, Singleton instantiation, GATT Characteristic updates,
// and extreme stress testing of the telemetry broadcaster to prevent radio stack buffer overflows.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>

#include "streaming/BLEManager.h"
#include "broadcast/GattConfigurator.hpp"
#include "broadcast/DataBroadcaster.hpp"
#include "broadcast/BleConnection.hpp"

BLEManager bleManager;

void setUp(void) {
    // No per-test setup required.
}

void tearDown(void) {
    // No per-test teardown required.
}

// ----------------------------------------------------------------------------------------------------
// Test 1: Verify the physical BLE radio and Facade orchestrator initialize without errors.
// ----------------------------------------------------------------------------------------------------
void test_ble_initialization(void) {
    bool initSuccess = bleManager.begin();
    TEST_ASSERT_TRUE_MESSAGE(initSuccess, "BLE Manager failed to initialize. Check hardware radio module.");
}

// ----------------------------------------------------------------------------------------------------
// Test 2: Ensure GATT Characteristics are correctly booted with safe default values (zeros).
// ----------------------------------------------------------------------------------------------------
void test_gatt_default_values(void) {
    uint32_t steps = GattConfigurator::getInstance().getStepsChar().value();
    uint32_t jumps = GattConfigurator::getInstance().getJumpsChar().value();
    uint32_t stairs = GattConfigurator::getInstance().getStairsChar().value();
    uint8_t state = GattConfigurator::getInstance().getStateChar().value();

    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, steps, "Initial step count should be 0");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, jumps, "Initial jump count should be 0");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, stairs, "Initial stair count should be 0");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(static_cast<uint8_t>(MotionState::IDLE), state, "Initial state should be IDLE");
}

// ----------------------------------------------------------------------------------------------------
// Test 3: Validate the DataBroadcaster Singleton successfully routes data to the GATT memory.
// ----------------------------------------------------------------------------------------------------
void test_broadcaster_updates_characteristics(void) {
    // Simulate a motion event pushing data to the broadcaster
    DataBroadcaster::getInstance().updateCounters(150, 5, 12);
    DataBroadcaster::getInstance().updateState(MotionState::STEP);

    // Read back directly from the GATT server characteristics
    uint32_t steps = GattConfigurator::getInstance().getStepsChar().value();
    uint32_t jumps = GattConfigurator::getInstance().getJumpsChar().value();
    uint32_t stairs = GattConfigurator::getInstance().getStairsChar().value();
    uint8_t state = GattConfigurator::getInstance().getStateChar().value();

    TEST_ASSERT_EQUAL_UINT32(150, steps);
    TEST_ASSERT_EQUAL_UINT32(5, jumps);
    TEST_ASSERT_EQUAL_UINT32(12, stairs);
    TEST_ASSERT_EQUAL_UINT8(static_cast<uint8_t>(MotionState::STEP), state);
}

// ----------------------------------------------------------------------------------------------------
// Test 4: Radio Buffer Stress Test - Pumping 10,000 rapid updates into the BLE Stack
// Purpose: Ensures the BLE Stack does not suffer from buffer overflow or hard faults during extreme
// continuous physical activity.
// ----------------------------------------------------------------------------------------------------
void test_broadcaster_stress_endurance(void) {
    // Attempt 10,000 rapid characteristic writes, simulating heavy activity
    for (uint32_t i = 1; i <= 10000; i++) {
        DataBroadcaster::getInstance().updateCounters(i, 0, 0);
        // Force the BLE stack to process the queued internal events
        bleManager.poll();
    }

    // Retrieve the final state directly from the GATT structure
    uint32_t finalSteps = GattConfigurator::getInstance().getStepsChar().value();

    // Verify 100% data integrity post-stress
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(10000, finalSteps, "BLE Broadcaster dropped packets during stress execution.");
}

void setup() {
    delay(2000); // Allow hardware radio and serial port to stabilize
    UNITY_BEGIN();
    
    RUN_TEST(test_ble_initialization);
    RUN_TEST(test_gatt_default_values);
    RUN_TEST(test_broadcaster_updates_characteristics);
    RUN_TEST(test_broadcaster_stress_endurance); // Execute the new stress test
    
    UNITY_END();
    delay(1000); // Ensure serial buffer flushes before reset
}

void loop() {
    // Unity test framework requires an empty loop
}