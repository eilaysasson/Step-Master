// ========================================================================================================
// File: test_ble_services.cpp
// Purpose: Validates the BLE Manager Facade, Singleton instantiation, and GATT Characteristic updates.
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

void setup() {
    delay(2000); // Allow hardware radio and serial port to stabilize
    UNITY_BEGIN();
    
    RUN_TEST(test_ble_initialization);
    RUN_TEST(test_gatt_default_values);
    RUN_TEST(test_broadcaster_updates_characteristics);
    
    UNITY_END();
    delay(1000); // Ensure serial buffer flushes before reset
}

void loop() {
    // Unity test framework requires an empty loop
}