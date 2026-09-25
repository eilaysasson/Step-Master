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

    constexpr uint32_t STEP_REFRACTORY_MS = 120;
    constexpr float STEP_THRESHOLD_MIN_G = 1.15f;
    constexpr float STEP_THRESHOLD_MAX_G = 1.90f;
    constexpr float STEP_K_FACTOR = 1.5f;

    constexpr float FLIGHT_ENTER_G = 0.35f;
    constexpr float FLIGHT_EXIT_G = 0.70f;
    constexpr uint32_t FLIGHT_MIN_DURATION_MS = 180;
    constexpr uint32_t FLIGHT_MAX_DURATION_MS = 850;
    constexpr uint32_t JUMP_CONSECUTIVE_FRAMES = 3;

    constexpr float STAIR_ENERGY_THRESHOLD = 0.45f;
    constexpr float STAIR_PITCH_THRESHOLD = 35.0f;
    constexpr uint32_t STAIR_CONSECUTIVE_FRAMES = 3;
}

#endif // ALGO_PARAMS_HPP