#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <cstdint>

/// Motion data sample containing acceleration + gyro and metadata.
struct MotionData
{
    float accelX;
    float accelY;
    float accelZ;

    float gyroX;
    float gyroY;
    float gyroZ;

    uint32_t sequence;
    uint32_t timestampMs;

    bool isStep;
};

/// Abstract interface for motion sensor implementations.
class MotionSensor
{
public:

    /// Initialize the sensor hardware.
    virtual bool begin() = 0;

    /// Read the latest motion sample into the provided output object.
    virtual bool read(MotionData& out) = 0;

    /// Return the number of I2C communication errors detected.
    virtual uint32_t i2cErrorCount() const = 0;
};


#endif