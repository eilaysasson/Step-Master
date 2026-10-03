// ========================================================================================================
// File: test_time_service.cpp
// Purpose: Validates time anchoring, Unix Epoch translations, and Session ID continuity across reboots.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "storage/TimeManager.hpp"
#include "MockStateStorage.hpp"

using namespace StepMaster::Storage;
using namespace StepMaster::Storage::Test;

// Allocated globally to persist data across simulated reboots
MockStateStorage stateStorage;
TimeManager* timeManager;

void setUp(void) {
    // We intentionally DO NOT clear stateStorage here so we can simulate reboots between tests
}

void tearDown(void) {
    delete timeManager;
}

void test_fresh_boot_creates_session_1() {
    stateStorage.isFormatted = false; // Force a factory reset
    timeManager = new TimeManager(stateStorage);
    
    TEST_ASSERT_TRUE(timeManager->begin());
    TEST_ASSERT_EQUAL_UINT32(1, timeManager->getCurrentSessionId());
}

void test_reboot_increments_session() {
    // The previous test left the stateStorage intact (Session 1)
    // Creating a new TimeManager simulates a hard reboot reading from the same Flash
    timeManager = new TimeManager(stateStorage);
    
    TEST_ASSERT_TRUE(timeManager->begin());
    TEST_ASSERT_EQUAL_UINT32(2, timeManager->getCurrentSessionId());
}

void test_epoch_sync_and_time_progression() {
    timeManager = new TimeManager(stateStorage);
    timeManager->begin();
    
    // Initially unsynced. Circumventing 64-bit Unity limit using standard boolean check.
    TEST_ASSERT_TRUE(timeManager->getUnixEpoch() == 0); 
    
    uint64_t simulatedPhoneTimeMs = 1700000000000ULL; // Standard Unix Timestamp
    timeManager->syncEpoch(simulatedPhoneTimeMs);
    
    delay(50); // Advance uptime by 50ms
    
    uint64_t currentTime = timeManager->getUnixEpoch();
    
    // Fix: Account for RTOS timer resolution jitter. delay(50) may register as ~49ms internally.
    TEST_ASSERT_TRUE(currentTime >= simulatedPhoneTimeMs + 40);
    TEST_ASSERT_TRUE(currentTime < simulatedPhoneTimeMs + 150);
}

// === GLOBAL SCOPE FUNCTIONS ===
void setup() {
    delay(2000);
    UNITY_BEGIN();
    
    RUN_TEST(test_fresh_boot_creates_session_1);
    RUN_TEST(test_reboot_increments_session);
    RUN_TEST(test_epoch_sync_and_time_progression);
    
    UNITY_END();
}

void loop() {
    delay(100);
}