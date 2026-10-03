// ========================================================================================================
// File: test_offline_storage.cpp
// Purpose: Validates the M4.1 Storage Contracts (Recovery, Power-Loss, CRC, and Pagination).
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "MockEventStorage.hpp"

using namespace StepMaster::Storage;
using namespace StepMaster::Storage::Test;

// Allocated in global memory once (takes ~130KB of RAM)
MockEventStorage storage;

void setUp(void) {
    storage.format(); // Reset storage to factory state before every test
}

void tearDown(void) {}

void test_format_and_initial_state() {
    uint32_t oldest, newest;
    bool hasData = storage.getRange(oldest, newest);
    
    // Freshly formatted storage should have no complete records
    TEST_ASSERT_FALSE(hasData);
    TEST_ASSERT_EQUAL(0, storage.activePageIdx);
    TEST_ASSERT_EQUAL(0, storage.activeRecordIdx);
}

void test_append_and_read_records() {
    LogRecord rec1 = {1001, 1, 5000, 1, 0, 0, 0}; // seq: 1001, type: STEP
    LogRecord rec2 = {1002, 1, 6000, 2, 0, 0, 0}; // seq: 1002, type: JUMP

    TEST_ASSERT_TRUE(storage.appendRecord(rec1));
    TEST_ASSERT_TRUE(storage.appendRecord(rec2));

    uint32_t oldest, newest;
    TEST_ASSERT_TRUE(storage.getRange(oldest, newest));
    TEST_ASSERT_EQUAL_UINT32(1001, oldest);
    TEST_ASSERT_EQUAL_UINT32(1002, newest);

    LogRecord batch[5];
    size_t count = 0;
    storage.readBatch(1001, batch, 5, count);
    
    TEST_ASSERT_EQUAL_size_t(2, count);
    TEST_ASSERT_EQUAL_UINT32(1001, batch[0].sequence);
    TEST_ASSERT_EQUAL_UINT8(1, batch[0].eventType);
    TEST_ASSERT_EQUAL_UINT32(1002, batch[1].sequence);
}

void test_power_loss_recovery_ignores_incomplete_record() {
    // Write 2 valid records
    LogRecord rec1 = {1001, 1, 5000, 1, 0, 0, 0};
    LogRecord rec2 = {1002, 1, 6000, 1, 0, 0, 0};
    storage.appendRecord(rec1);
    storage.appendRecord(rec2);

    // Simulate power loss on the 3rd record
    LogRecord rec3 = {1003, 1, 7000, 1, 0, 0, 0};
    storage.simulatePowerLoss = true; 
    storage.appendRecord(rec3); // This will write 0xFFFFFFFF to commitWord

    // Simulate device reboot by dropping the RAM state (but keeping the array memory intact)
    storage.activePageIdx = 0;
    storage.activeRecordIdx = 0;
    
    TEST_ASSERT_TRUE(storage.initAndRecover());
    
    // Recovery should stop at record 2, ignoring the incomplete record 3
    TEST_ASSERT_EQUAL(2, storage.activeRecordIdx);
    
    uint32_t oldest, newest;
    storage.getRange(oldest, newest);
    TEST_ASSERT_EQUAL_UINT32(1002, newest); // Sequence 1003 is safely ignored
}

void test_crc_corruption_rejection() {
    LogRecord rec1 = {1001, 1, 5000, 1, 0, 0, 0};
    storage.appendRecord(rec1);

    LogRecord rec2 = {1002, 1, 6000, 1, 0, 0, 0};
    storage.simulateCrcCorruption = true;
    storage.appendRecord(rec2); // Record written but with bad CRC

    // Simulate device reboot
    storage.activePageIdx = 0;
    storage.activeRecordIdx = 0;
    storage.initAndRecover();
    
    // Recovery should ignore record 2 due to bad CRC
    TEST_ASSERT_EQUAL(1, storage.activeRecordIdx);
}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_format_and_initial_state);
    RUN_TEST(test_append_and_read_records);
    RUN_TEST(test_power_loss_recovery_ignores_incomplete_record);
    RUN_TEST(test_crc_corruption_rejection);
    UNITY_END();
}

void loop() {
    delay(100);
}