// ========================================================================================================
// File: test_sync_manager.cpp
// Purpose: Validates Resumable Bulk Sync logic, Cumulative ACKs, and Batch progression.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "storage/SyncManager.hpp"
#include <vector>

using namespace StepMaster::Storage;

// --- Minimal Mocks for TDD ---
class MockEventStorage : public IEventStorage {
public:
    std::vector<LogRecord> records;
    
    bool initAndRecover() override { return true; }
    bool appendRecord(const LogRecord& record) override {
        records.push_back(record);
        return true;
    }
    bool getRange(uint32_t& oldest, uint32_t& newest) const override {
        if (records.empty()) return false;
        oldest = records.front().sequence;
        newest = records.back().sequence;
        return true;
    }
    bool readBatch(uint32_t startSeq, LogRecord* outRecords, size_t cap, size_t& count) override {
        count = 0;
        for (const auto& r : records) {
            // Using sequence comparison to find records belonging to the requested batch
            if (!isSequenceNewer(startSeq, r.sequence) && count < cap) {
                outRecords[count++] = r;
            }
        }
        return count > 0;
    }
    bool format() override { records.clear(); return true; }
    bool eraseAll() override { return format(); }
};

class MockBulkTransport : public IBulkTransport {
public:
    std::vector<LogRecord> sentRecords;
    bool shouldFail = false;

    bool sendBatch(const LogRecord* records, size_t count) override {
        if (shouldFail) return false;
        for (size_t i = 0; i < count; i++) {
            sentRecords.push_back(records[i]);
        }
        return true;
    }
};

// --- Test Cases ---
void test_sync_starts_batches_and_completes() {
    MockEventStorage storage;
    MockBulkTransport transport;
    SyncManager syncManager(storage, transport);

    // Simulate 12 historical events logged offline
    for (uint32_t i = 1; i <= 12; i++) {
        storage.appendRecord({i, 1, i*100, 1, 0, 0, 0});
    }

    syncManager.beginSync(1);
    TEST_ASSERT_TRUE(syncManager.isSyncInProgress());

    // 12 records at 5 per batch = 3 poll cycles
    syncManager.poll(); // Sent 1-5
    syncManager.poll(); // Sent 6-10
    syncManager.poll(); // Sent 11-12
    
    TEST_ASSERT_EQUAL_size_t(12, transport.sentRecords.size());
    TEST_ASSERT_EQUAL_UINT32(12, transport.sentRecords.back().sequence);

    // App acknowledges processing up to record 12
    syncManager.acknowledge(12);
    TEST_ASSERT_FALSE(syncManager.isSyncInProgress()); // Sync completed
}

void test_sync_resumes_cleanly_after_disconnect() {
    MockEventStorage storage;
    MockBulkTransport transport;
    SyncManager syncManager(storage, transport);

    for (uint32_t i = 1001; i <= 1010; i++) {
        storage.appendRecord({i, 1, i*100, 1, 0, 0, 0});
    }

    syncManager.beginSync(1001);
    syncManager.poll(); // Sends 1001-1005
    
    TEST_ASSERT_EQUAL_size_t(5, transport.sentRecords.size());
    
    // Simulate App ACKing up to 1003, then BLE disconnects abruptly
    syncManager.acknowledge(1003);
    
    // App reconnects, logically requesting to resume from 1004
    syncManager.beginSync(1004);
    syncManager.poll(); // Sends 1004-1008
    syncManager.poll(); // Sends 1009-1010
    
    // Total physical transmissions = 5 (first batch) + 5 + 2 = 12 operations
    TEST_ASSERT_EQUAL_size_t(12, transport.sentRecords.size()); 
    TEST_ASSERT_EQUAL_UINT32(1010, transport.sentRecords.back().sequence);
    
    syncManager.acknowledge(1010);
    TEST_ASSERT_FALSE(syncManager.isSyncInProgress());
}

// === GLOBAL SCOPE FUNCTIONS ===
void setUp(void) {}
void tearDown(void) {}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    
    RUN_TEST(test_sync_starts_batches_and_completes);
    RUN_TEST(test_sync_resumes_cleanly_after_disconnect);
    
    UNITY_END();
}

void loop() {
    delay(100);
}