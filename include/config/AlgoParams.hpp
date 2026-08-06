#ifndef ALGO_PARAMS_HPP
#define ALGO_PARAMS_HPP

#include <cstddef>
#include <cstdint>

namespace AlgoParams
{

/// Target sample rate in Hertz used by the application.
constexpr float SAMPLE_RATE_HZ = 100.0f;

/// Milliseconds per sample, derived from the sample rate.
constexpr float TS_MS = 10.0f;

/// Sampling interval in microseconds for the hardware timer.
constexpr uint32_t SAMPLE_INTERVAL_US = 10000;

/// Capacity of the ring buffer used to queue raw samples.
constexpr std::size_t RING_BUFFER_CAPACITY = 64;

/// Serial baud rate for console streaming and CLI communication.
constexpr uint32_t SERIAL_BAUD = 115200;

/// IMU output data rate in Hertz.
constexpr uint16_t IMU_ODR_HZ = 104;

/// Accelerometer full-scale range in g units.
constexpr uint16_t IMU_ACCEL_RANGE_G = 16;

/// Gyroscope full-scale range in degrees per second.
constexpr uint16_t IMU_GYRO_RANGE_DPS = 2000;

/// I2C bus clock frequency used to communicate with the IMU.
constexpr uint32_t IMU_I2C_CLOCK_HZ = 400000;

/// Default I2C address for the LSM6DS3 IMU sensor.
constexpr uint8_t IMU_I2C_ADDRESS = 0x6A;

/// Interval used to print periodic statistics over serial.
constexpr uint32_t STAT_INTERVAL_MS = 10000;

} // namespace AlgoParams

#endif
