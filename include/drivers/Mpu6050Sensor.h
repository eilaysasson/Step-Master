#ifndef MPU6050_SENSOR_H
#define MPU6050_SENSOR_H

#include "MotionSensor.h"

#if !USE_MOCK_SENSOR
/// Concrete motion sensor implementation for the MPU6050 IMU.
class Mpu6050Sensor : public MotionSensor
{
public:
    bool begin() override;
    bool read(MotionData& out) override;
    uint32_t i2cErrorCount() const override;

private:
    uint32_t i2cErrors_ = 0;
    bool ready_ = false;
};
#endif
#endif