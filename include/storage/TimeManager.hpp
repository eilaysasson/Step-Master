// ========================================================================================================
// File: TimeManager.hpp
// Purpose: Implements ITimeService. Manages Session IDs across reboots and bridges monotonic uptime 
// to real-world Unix Epoch time using a dynamic anchor point.
// ========================================================================================================

#ifndef TIME_MANAGER_HPP
#define TIME_MANAGER_HPP

#include "storage/StorageInterfaces.hpp"
#include <Arduino.h>

namespace StepMaster {
namespace Storage {

    class TimeManager : public ITimeService {
    public:
        // Dependency Injection: Needs the StateStorage to persist the anchor and session
        TimeManager(IStateStorage& stateStorage) : stateStorage_(stateStorage) {}

        bool begin() {
            // Load existing state from Flash/Mock
            if (!stateStorage_.loadState(state_)) {
                // If storage is unformatted or corrupt, initialize a fresh state
                state_ = PersistentState{0};
            }
            
            // Increment Session ID to logically separate events before/after power cycles
            state_.sessionId++;
            
            // Save immediately so the new session is committed
            return stateStorage_.saveState(state_);
        }

        uint32_t getMonotonicMillis() override {
            return millis();
        }

        uint64_t getUnixEpoch() override {
            if (state_.epochAnchor == 0) {
                return 0; // Device has not been synced with the phone yet
            }
            
            uint32_t currentMillis = getMonotonicMillis();
            uint32_t elapsedMs = currentMillis - state_.monotonicAnchor;
            
            return state_.epochAnchor + elapsedMs;
        }

        void syncEpoch(uint64_t newEpochTimeMs) override {
            state_.epochAnchor = newEpochTimeMs;
            state_.monotonicAnchor = getMonotonicMillis();
            stateStorage_.saveState(state_);
        }

        uint32_t getCurrentSessionId() const override {
            return state_.sessionId;
        }

        // Direct access to state for other components (e.g., updating total steps)
        PersistentState& getState() { return state_; }
        
        bool saveState() { return stateStorage_.saveState(state_); }

    private:
        IStateStorage& stateStorage_;
        PersistentState state_{};
    };

} // namespace Storage
} // namespace StepMaster

#endif // TIME_MANAGER_HPP