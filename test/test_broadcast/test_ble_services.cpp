// ========================================================================================================
// File: test_ble_services.cpp
// Purpose: Validates the BLE Manager Facade, Singleton instantiation, and GATT String Characteristic updates.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>

#include "streaming/BLEManager.h"
#include "broadcast/GattConfigurator.hpp"
#include "broadcast/DataBroadcaster.hpp"
#include "broadcast/BleConnection.hpp"

BLEManager bleManager;

void setUp(void) {}
void tearDown(void) {}

void test_ble_initialization(void) {
    bool initSuccess = bleManager.begin();
    TEST_ASSERT_TRUE_MESSAGE(initSuccess, "BLE Manager failed to initialize. Check hardware radio module.");
}

void test_gatt_default_values(void) {
    // FIX: Using .toInt() to convert the BLEStringCharacteristic value back to integers for assertion
    uint32_t steps = GattConfigurator::getInstance().getStepsChar().value().toInt();
    uint32_t jumps = GattConfigurator::getInstance().getJumpsChar().value().toInt();
    uint32_t stairs = GattConfigurator::getInstance().getStairsChar().value().toInt();
    String state = GattConfigurator::getInstance().getStateChar().value();

    TEST_ASSERT_EQUAL_UINT32(0, steps);
    TEST_ASSERT_EQUAL_UINT32(0, jumps);
    TEST_ASSERT_EQUAL_UINT32(0, stairs);
    TEST_ASSERT_EQUAL_STRING("IDLE", state.c_str());
}

void test_broadcaster_updates_characteristics(void) {
    DataBroadcaster::getInstance().updateCounters(150, 5, 12);
    DataBroadcaster::getInstance().updateState(MotionState::STEP);

    // FIX: Using .toInt() to convert the BLEStringCharacteristic value back to integers for assertion
    uint32_t steps = GattConfigurator::getInstance().getStepsChar().value().toInt();
    uint32_t jumps = GattConfigurator::getInstance().getJumpsChar().value().toInt();
    uint32_t stairs = GattConfigurator::getInstance().getStairsChar().value().toInt();
    String state = GattConfigurator::getInstance().getStateChar().value();

    TEST_ASSERT_EQUAL_UINT32(150, steps);
    TEST_ASSERT_EQUAL_UINT32(5, jumps);
    TEST_ASSERT_EQUAL_UINT32(12, stairs);
    TEST_ASSERT_EQUAL_STRING("STEP", state.c_str());
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    
    RUN_TEST(test_ble_initialization);
    RUN_TEST(test_gatt_default_values);
    RUN_TEST(test_broadcaster_updates_characteristics);
    
    UNITY_END();
}

void loop() {}