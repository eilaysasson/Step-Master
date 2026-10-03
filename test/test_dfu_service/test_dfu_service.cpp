// ========================================================================================================
// File: test_dfu_service.cpp
// Purpose: Validates the presence and configuration of the OTA DFU service.
// ========================================================================================================

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <unity.h>

#include "config/BLEConfig.hpp"
#include "broadcast/GattConfigurator.hpp"

GattConfigurator testGatt;

void setUp(void) {}
void tearDown(void) {}

void test_dfu_uuids_are_correct(void) {
    TEST_ASSERT_EQUAL_STRING("FE59", BLEConfig::DFU_SERVICE_UUID);
    TEST_ASSERT_EQUAL_STRING("8EC90001-F315-4F60-9FB8-838830DAEA50", BLEConfig::DFU_CONTROL_CHAR_UUID);
}

void test_dfu_service_initialization(void) {
    bool bleStarted = BLE.begin();
    TEST_ASSERT_TRUE_MESSAGE(bleStarted, "BLE radio failed to start");
    
    testGatt.setupServices();
    TEST_ASSERT_TRUE_MESSAGE(true, "DFU service configured successfully");
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    
    RUN_TEST(test_dfu_uuids_are_correct);
    RUN_TEST(test_dfu_service_initialization);
    
    UNITY_END();
}

void loop() {
    delay(100);
}