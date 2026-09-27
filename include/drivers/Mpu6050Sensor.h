// ========================================================================================================
// File: Mpu6050Sensor.h
// Purpose: Concrete motion sensor implementation for the MPU6050 IMU.
// ========================================================================================================

#ifndef MPU6050_SENSOR_H
#define MPU6050_SENSOR_H

#include "sensors/MotionSensor.h"

#if !USE_MOCK_SENSOR
class Mpu6050Sensor : public MotionSensor
{
public:
    bool begin() override;
    bool read(MotionData& out) override;
    uint32_t i2cErrorCount() const override;

private:
    uint32_t i2cErrors_ = 0;
    uint32_t sequence_ = 0; // Tracks the absolute number of physical samples read
    bool ready_ = false;
};
#endif // !USE_MOCK_SENSOR
#endif // MPU6050_SENSOR_H