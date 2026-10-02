// ========================================================================================================
// File: TimeGuard.hpp
// Purpose: A reusable debouncer/cooldown utility to manage refractory periods for motion detectors.
// ========================================================================================================

#ifndef TIME_GUARD_HPP
#define TIME_GUARD_HPP

#include <cstdint>

class TimeGuard {
public:
    TimeGuard() : cooldownEndMs_(0) {}

    // Checks if the cooldown period has completely elapsed.
    bool isReady(uint32_t currentTimestampMs) const {
        return currentTimestampMs >= cooldownEndMs_;
    }

    // Sets a new cooldown target based on the current time and desired duration.
    void setCooldown(uint32_t currentTimestampMs, uint32_t durationMs) {
        cooldownEndMs_ = currentTimestampMs + durationMs;
    }

private:
    uint32_t cooldownEndMs_;
};

#endif // TIME_GUARD_HPP