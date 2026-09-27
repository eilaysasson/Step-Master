// ========================================================================================================
// File: Mpu6050Sensor.cpp
// Purpose: Driver implementation for MPU6050 IMU using the Adafruit library.
// ========================================================================================================

#include "drivers/Mpu6050Sensor.h"
#include "params/AlgoParams.hpp"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#if !USE_MOCK_SENSOR

namespace {
    Adafruit_MPU6050 mpu;
}

bool Mpu6050Sensor::begin() {
    Wire.begin();
    
    ready_ = mpu.begin(AlgoParams::IMU_I2C_ADDRESS, &Wire, 0);
    
    if (!ready_) {
        ++i2cErrors_;
        return false;
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    return true;
}

bool Mpu6050Sensor::read(MotionData& out) {
    if (!ready_) {
        ++i2cErrors_;
        return false;
    }

    sensors_event_t a, g, temp;
    
    // Fetch fresh data from the physical I2C sensor
    if (!mpu.getEvent(&a, &g, &temp)) {
        ++i2cErrors_;
        return false;
    }

    // Convert m/s^2 to standard G units (1G = 9.80665 m/s^2)
    out.accelX = a.acceleration.x / 9.80665f;
    out.accelY = a.acceleration.y / 9.80665f;
    out.accelZ = a.acceleration.z / 9.80665f;
    
    // Convert radians/s to degrees/s
    out.gyroX = g.gyro.x * 57.2958f;
    out.gyroY = g.gyro.y * 57.2958f;
    out.gyroZ = g.gyro.z * 57.2958f;
    
    // Assign sequential ID and increment
    out.sequence = sequence_++;
    
    // Timestamp is explicitly handled by the main loop hardware timer
    out.timestampMs = 0;

    return true;
}

uint32_t Mpu6050Sensor::i2cErrorCount() const {
    return i2cErrors_;
}

#endif // !USE_MOCK_SENSOR