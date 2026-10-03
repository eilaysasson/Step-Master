// ========================================================================================================
// File: StorageInterfaces.hpp
// Purpose: v1.0 M4.0-GATE - Abstract interfaces for offline storage and synchronization.
// ========================================================================================================

#ifndef STORAGE_INTERFACES_HPP
#define STORAGE_INTERFACES_HPP

#include "DataContracts.hpp"
#include <cstddef>

namespace StepMaster::Storage {

    // ----------------------------------------------------------------------------------------------------
    // Interface: IEventStorage
    // Purpose: Manages the Wear-Leveled Ring Buffer for LogRecords.
    // ----------------------------------------------------------------------------------------------------
    class IEventStorage {
    public:
        virtual ~IEventStorage() = default;
        
        virtual bool initAndRecover() = 0;
        virtual bool appendRecord(const LogRecord& record) = 0;
        
        // Sync API: Fetch range to coordinate with the mobile app
        virtual bool getRange(uint32_t& oldestSequence, uint32_t& newestSequence) const = 0;
        
        // Sync API: Efficient batch reading for high-throughput BLE transmission
        virtual bool readBatch(uint32_t startSequence, LogRecord* outRecords, size_t capacity, size_t& count) = 0;
        
        // Destructive operations return status to handle hardware failures
        virtual bool format() = 0;
        virtual bool eraseAll() = 0;
    };

    // ----------------------------------------------------------------------------------------------------
    // Interface: IStateStorage
    // Purpose: Manages the periodic checkpoints of the overall counters.
    // ----------------------------------------------------------------------------------------------------
    class IStateStorage {
    public:
        virtual ~IStateStorage() = default;
        
        virtual bool loadState(PersistentState& outState) = 0;
        virtual bool saveState(const PersistentState& state) = 0;
    };

    // ----------------------------------------------------------------------------------------------------
    // Interface: ITimeService
    // Purpose: Separates monotonic intervals from human-readable wall clock time.
    // ----------------------------------------------------------------------------------------------------
    class ITimeService {
    public:
        virtual ~ITimeService() = default;
        
        virtual uint32_t getMonotonicMillis() = 0;
        virtual uint64_t getUnixEpoch() = 0;
        virtual void syncEpoch(uint64_t newEpochTime) = 0;
        virtual uint32_t getCurrentSessionId() const = 0;
    };

    // ----------------------------------------------------------------------------------------------------
    // Interface: ISyncManager
    // Purpose: Handles the Resumable Bulk Sync protocol over BLE using Cumulative ACKs.
    // ----------------------------------------------------------------------------------------------------
    class ISyncManager {
    public:
        virtual ~ISyncManager() = default;
        
        virtual void beginSync(uint32_t fromSequence) = 0;
        
        // Cumulative ACK: Confirms that all records <= sequenceId were durably accepted by the app
        virtual void acknowledge(uint32_t sequenceId) = 0;
        virtual bool isSyncInProgress() const = 0;
    };

} // namespace StepMaster::Storage

#endif // STORAGE_INTERFACES_HPP