// ========================================================================================================
// File: MockEventStorage.hpp
// Purpose: In-memory simulation of Flash storage for testing Wear-Leveling, CRC, and Recovery logic.
// ========================================================================================================

#ifndef MOCK_EVENT_STORAGE_HPP
#define MOCK_EVENT_STORAGE_HPP

#include "storage/StorageInterfaces.hpp"
#include <cstring>
#include <vector>

namespace StepMaster::Storage::Test {

    class MockEventStorage : public IEventStorage {
    public:
        static constexpr uint32_t PAGE_SIZE = 4096;
        static constexpr uint32_t NUM_PAGES = 32;
        static constexpr uint32_t RECORDS_PER_PAGE = (PAGE_SIZE - sizeof(PageHeader)) / sizeof(LogRecord);

        struct MockPage {
            PageHeader header;
            LogRecord records[RECORDS_PER_PAGE];
        };

        MockPage memory[NUM_PAGES];
        uint32_t activePageIdx = 0;
        uint32_t activeRecordIdx = 0;
        bool simulatePowerLoss = false;
        bool simulateCrcCorruption = false;

        MockEventStorage() { eraseAll(); }

        bool format() override {
            return eraseAll();
        }

        bool eraseAll() override {
            memset(memory, 0xFF, sizeof(memory)); // Flash default erased state is 0xFF
            activePageIdx = 0;
            activeRecordIdx = 0;
            
            // Format first page
            memory[0].header.magic = PAGE_VALIDATION_SIGNATURE;
            memory[0].header.formatVersion = 1;
            memory[0].header.pageSequence = 1;
            memory[0].header.firstRecordSequence = 1;
            memory[0].header.crc32 = calculateHeaderCrc(memory[0].header);
            return true;
        }

        bool initAndRecover() override {
            uint32_t highestPageSeq = 0;
            int bestPageIdx = -1;

            // 1. Find the active page (highest valid pageSequence)
            for (uint32_t i = 0; i < NUM_PAGES; i++) {
                if (memory[i].header.magic == PAGE_VALIDATION_SIGNATURE) {
                    if (memory[i].header.crc32 == calculateHeaderCrc(memory[i].header)) {
                        if (bestPageIdx == -1 || isSequenceNewer(memory[i].header.pageSequence, highestPageSeq)) {
                            highestPageSeq = memory[i].header.pageSequence;
                            bestPageIdx = i;
                        }
                    }
                }
            }

            if (bestPageIdx == -1) {
                return format(); // No valid pages found, factory reset
            }

            activePageIdx = bestPageIdx;

            // 2. Scan records in the active page to find the next write pointer
            activeRecordIdx = 0;
            for (uint32_t i = 0; i < RECORDS_PER_PAGE; i++) {
                const LogRecord& rec = memory[activePageIdx].records[i];
                if (rec.commitWord == 0xFFFFFFFF) break; // Reached erased memory
                
                if (rec.commitWord == ATOMIC_COMMIT_FLAG) {
                    if (rec.crc16 == calculateRecordCrc(rec)) {
                        activeRecordIdx = i + 1; // Valid record, move pointer forward
                    } else {
                        break; // Corrupted CRC! Stop recovery here.
                    }
                } else {
                    break; // Power loss detected (incomplete commit). Stop recovery here.
                }
            }

            // If page is full, rotate to next page
            if (activeRecordIdx >= RECORDS_PER_PAGE) {
                rotatePage();
            }

            return true;
        }

        bool appendRecord(const LogRecord& record) override {
            if (activeRecordIdx >= RECORDS_PER_PAGE) {
                rotatePage();
            }

            LogRecord copy = record;
            copy.crc16 = calculateRecordCrc(copy);
            
            if (simulateCrcCorruption) {
                copy.crc16 ^= 0xFFFF; // Break CRC
                simulateCrcCorruption = false;
            }

            if (simulatePowerLoss) {
                copy.commitWord = 0xFFFFFFFF; // Simulate power dying before commit word is written
                simulatePowerLoss = false;
            } else {
                copy.commitWord = ATOMIC_COMMIT_FLAG;
            }

            memory[activePageIdx].records[activeRecordIdx] = copy;
            activeRecordIdx++;
            return true;
        }

        bool getRange(uint32_t& oldestSequence, uint32_t& newestSequence) const override {
            if (activeRecordIdx == 0) return false;
            newestSequence = memory[activePageIdx].records[activeRecordIdx - 1].sequence;
            // CORRECTED: Read actual record sequence, not the hardcoded page header
            oldestSequence = memory[activePageIdx].records[0].sequence; 
            return true;
        }

        bool readBatch(uint32_t startSequence, LogRecord* outRecords, size_t capacity, size_t& count) override {
            count = 0;
            for (uint32_t i = 0; i < activeRecordIdx && count < capacity; i++) {
                if (memory[activePageIdx].records[i].sequence >= startSequence) {
                    outRecords[count++] = memory[activePageIdx].records[i];
                }
            }
            return count > 0;
        }

    private:
        void rotatePage() {
            uint32_t newPageSeq = memory[activePageIdx].header.pageSequence + 1;
            uint32_t nextRecSeq = memory[activePageIdx].records[RECORDS_PER_PAGE - 1].sequence + 1;
            
            activePageIdx = (activePageIdx + 1) % NUM_PAGES;
            activeRecordIdx = 0;
            
            // Erase new page
            memset(&memory[activePageIdx], 0xFF, sizeof(MockPage));
            
            // Setup new header
            memory[activePageIdx].header.magic = PAGE_VALIDATION_SIGNATURE;
            memory[activePageIdx].header.formatVersion = 1;
            memory[activePageIdx].header.pageSequence = newPageSeq;
            memory[activePageIdx].header.firstRecordSequence = nextRecSeq;
            memory[activePageIdx].header.crc32 = calculateHeaderCrc(memory[activePageIdx].header);
        }

        uint16_t calculateRecordCrc(const LogRecord& rec) const {
            // Dummy XOR CRC for testing purposes
            uint16_t crc = 0;
            crc ^= rec.sequence;
            crc ^= rec.timestampMs;
            crc ^= rec.eventType;
            return crc;
        }

        uint32_t calculateHeaderCrc(const PageHeader& hdr) const {
            return hdr.pageSequence ^ hdr.firstRecordSequence ^ hdr.magic;
        }
    };
}

#endif // MOCK_EVENT_STORAGE_HPP