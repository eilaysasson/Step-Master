// ========================================================================================================
// File: include/params/AlgoParams.hpp
// Purpose: Defines system-wide configuration constants for hardware, signal processing, and algorithms.
// ========================================================================================================

#ifndef ALGO_PARAMS_HPP
#define ALGO_PARAMS_HPP

#include <cstddef>
#include <cstdint>

namespace AlgoParams {
    constexpr float SAMPLE_RATE_HZ = 100.0f;
    constexpr float TS_MS = 10.0f;
    constexpr uint32_t SAMPLE_INTERVAL_US = 10000;
    
    constexpr std::size_t RING_BUFFER_CAPACITY = 64;
    constexpr uint32_t SERIAL_BAUD = 115200;
    
    constexpr uint16_t IMU_ACCEL_RANGE_G = 4;
    constexpr uint16_t IMU_GYRO_RANGE_DPS = 500;
    constexpr uint32_t IMU_I2C_CLOCK_HZ = 400000;
    constexpr uint8_t IMU_I2C_ADDRESS = 0x68;
    
    constexpr uint32_t STAT_INTERVAL_MS = 10000;
    constexpr uint32_t LED_PULSE_DURATION_MS = 200;

    constexpr std::size_t SLIDING_WINDOW_SIZE = 100;

    // --- Unified Kinematics (Pitch-Based Separation) ---
    constexpr uint32_t STEP_REFRACTORY_MS = 250;
    constexpr float STEP_THRESHOLD_MIN_G = 1.15f;
    constexpr float STEP_THRESHOLD_MAX_G = 1.90f;
    constexpr float STEP_K_FACTOR = 1.5f;

    // Pitch boundaries to strictly enforce mutual exclusion (no double counting).
    constexpr float PITCH_THRESHOLD_STAIRS = 25.0f; // Angles >= 25 but < 65 classify as stairs.
    constexpr float PITCH_THRESHOLD_JUMP = 65.0f;   // Angles >= 65 classify as jumps (knees-to-chest/high kicks).
}

#endif // ALGO_PARAMS_HPP