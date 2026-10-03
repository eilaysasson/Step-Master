// ========================================================================================================
// File: SyncManager.hpp
// Purpose: Implements ISyncManager to handle resumable offline data synchronization via Cumulative ACKs.
// ========================================================================================================

#ifndef SYNC_MANAGER_HPP
#define SYNC_MANAGER_HPP

#include "storage/StorageInterfaces.hpp"
#include <cstddef>

namespace StepMaster {
namespace Storage {

    // Abstraction for the underlying BLE transmission layer
    class IBulkTransport {
    public:
        virtual ~IBulkTransport() = default;
        // Transmits a batch of records. Returns true if queued successfully.
        virtual bool sendBatch(const LogRecord* records, size_t count) = 0;
    };

    class SyncManager : public ISyncManager {
    public:
        SyncManager(IEventStorage& storage, IBulkTransport& transport) 
            : storage_(storage), transport_(transport), 
              syncInProgress_(false), currentSequence_(0), lastAckedSequence_(0) {}

        void beginSync(uint32_t fromSequence) override {
            currentSequence_ = fromSequence;
            lastAckedSequence_ = fromSequence > 0 ? fromSequence - 1 : 0;
            syncInProgress_ = true;
        }

        void acknowledge(uint32_t sequenceId) override {
            if (isSequenceNewer(sequenceId, lastAckedSequence_)) {
                lastAckedSequence_ = sequenceId;
            }
            
            // Check if we reached the absolute newest available record in storage
            uint32_t oldest, newest;
            if (storage_.getRange(oldest, newest)) {
                if (!isSequenceNewer(newest, lastAckedSequence_)) {
                    syncInProgress_ = false; // Sync fully completed
                }
            } else {
                syncInProgress_ = false; // Storage is completely empty
            }
        }

        bool isSyncInProgress() const override {
            return syncInProgress_;
        }

        // Must be called periodically from the main loop to process pending data
        void poll() {
            if (!syncInProgress_) return;

            LogRecord batch[5];
            size_t count = 0;

            // Read the next sequential batch from storage
            if (storage_.readBatch(currentSequence_, batch, 5, count)) {
                if (transport_.sendBatch(batch, count)) {
                    // Advance pointer to the sequence following the last sent record
                    currentSequence_ = batch[count - 1].sequence + 1;
                }
            }
        }

    private:
        IEventStorage& storage_;
        IBulkTransport& transport_;
        bool syncInProgress_;
        uint32_t currentSequence_;
        uint32_t lastAckedSequence_;
    };

} // namespace Storage
} // namespace StepMaster

#endif // SYNC_MANAGER_HPP