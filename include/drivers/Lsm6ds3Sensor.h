#ifndef LSM6DS3_SENSOR_H
#define LSM6DS3_SENSOR_H

#include "MotionSensor.h"

/// Concrete motion sensor implementation for the LSM6DS3 IMU.
class Lsm6ds3Sensor : public MotionSensor
{
public:
    /// Initialize the IMU and configure accel/gyro settings.
    bool begin() override;

    /// Read accelerometer and gyroscope values.
    bool read(MotionData& out) override;

    /// Return the number of I2C errors recorded since startup.
    uint32_t i2cErrorCount() const override;

private:
    uint32_t i2cErrors_ = 0;
    bool ready_ = false;
};

#endif
