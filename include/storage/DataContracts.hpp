// ========================================================================================================
// File: DataContracts.hpp
// Purpose: v1.0 M4.0-GATE - Defines the precise memory layout for Flash persistence. 
// Enforces strict sizing, 32-bit alignment, and power-loss safety.
// ========================================================================================================

#ifndef DATA_CONTRACTS_HPP
#define DATA_CONTRACTS_HPP

#include <cstdint>

namespace StepMaster::Storage {

    // Magic numbers for validation and atomic commits
    constexpr uint32_t PAGE_VALIDATION_SIGNATURE = 0x53544550;     // "STEP"
    constexpr uint32_t ATOMIC_COMMIT_FLAG = 0xC01117ED; // COMMITTED

    // ----------------------------------------------------------------------------------------------------
    // Helper: Serial Arithmetic for Sequence Wraparound handling
    // Returns true if seqA is strictly newer than seqB, safely handling uint32_t overflow.
    // ----------------------------------------------------------------------------------------------------
    inline bool isSequenceNewer(uint32_t seqA, uint32_t seqB) {
        return static_cast<int32_t>(seqA - seqB) > 0;
    }

    // ----------------------------------------------------------------------------------------------------
    // 1. LogRecord (20 Bytes)
    // Purpose: Represents a single historical event (Step, Jump, Stair).
    // Capacity Model: At 20 bytes/record, a 128KB partition stores ~6,500 records.
    // Depending on event rate, this provides dynamic capacity (not a hardcoded "2-4 days").
    // ----------------------------------------------------------------------------------------------------
    struct alignas(4) LogRecord {
        uint32_t sequence;      // Monotonically increasing globally
        uint32_t sessionId;     // Links timestampMs to a specific boot epoch
        uint32_t timestampMs;   // Monotonic uptime of the event within the session
        uint8_t  eventType;     // Corresponds to MotionState enum
        uint8_t  eventData;     // Optional context (e.g., peak acceleration)
        uint16_t crc16;         // Protects the first 14 bytes (sequence to eventData)
        uint32_t commitWord;    // 32-bit flash-friendly commit marker (0xC0MM17ED)
    };
    static_assert(sizeof(LogRecord) == 20, "LogRecord must be exactly 20 bytes");
    static_assert(alignof(LogRecord) == 4, "LogRecord must be 32-bit aligned");

    // ----------------------------------------------------------------------------------------------------
    // 2. PageHeader (20 Bytes)
    // Purpose: Placed at the exact beginning of every 4KB Flash sector.
    // ----------------------------------------------------------------------------------------------------
    struct alignas(4) PageHeader {
        uint32_t magic;               // Must be PAGE_MAGIC_WORD
        uint16_t formatVersion;       // Allows future schema migrations (e.g., 0x01)
        uint16_t headerFlags;         // Status flags or reserved space
        uint32_t pageSequence;        // uint32_t to safely handle long-term rotation
        uint32_t firstRecordSequence; // The sequence ID of the first record in this page
        uint32_t crc32;               // Checksum of the header to verify structural integrity
    };
    static_assert(sizeof(PageHeader) == 20, "PageHeader must be exactly 20 bytes");
    static_assert(alignof(PageHeader) == 4, "PageHeader must be 32-bit aligned");

    // ----------------------------------------------------------------------------------------------------
    // 3. PersistentState (40 Bytes)
    // Purpose: A checkpoint saved periodically. Includes an explicit epoch and cumulative counters.
    // ----------------------------------------------------------------------------------------------------
    struct alignas(8) PersistentState {
        uint64_t epochAnchor;                // 8 bytes: True Unix time provided by BLE sync
        uint32_t totalSteps;                 // 4 bytes
        uint32_t totalJumps;                 // 4 bytes
        uint32_t totalStairs;                // 4 bytes
        uint32_t sessionId;                  // 4 bytes: Incremented on every device reboot
        uint32_t highestCommittedSequence;   // 4 bytes: Next sequence = highestCommittedSequence + 1
        uint32_t monotonicAnchor;            // 4 bytes: The device millis() when epochAnchor was set
        uint32_t reserved;                   // 4 bytes: Padding/Future use
        uint32_t crc32;                      // 4 bytes: Checksum of the entire state block
    };
    static_assert(sizeof(PersistentState) == 40, "PersistentState must be exactly 40 bytes");
    // Explicit alignas(8) used above because uint64_t forces 8-byte alignment on some ARM ABIs.

} // namespace StepMaster::Storage

#endif // DATA_CONTRACTS_HPP