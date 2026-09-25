#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <cstdint>

struct MotionData {
    float accelX;
    float accelY;
    float accelZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    uint32_t sequence;
    uint32_t timestampMs;
};

class MotionSensor {
public:
    virtual ~MotionSensor() = default;
    virtual bool begin() = 0;
    virtual bool read(MotionData& out) = 0;
    virtual uint32_t i2cErrorCount() const = 0;
};

#endif // MOTION_SENSOR_H