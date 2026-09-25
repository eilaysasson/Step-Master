#include "sensors/MockSensor.h"
#include <Arduino.h>
#include <cmath>

namespace
{
class MockSensorImpl : public MotionSensor
{
public:
    bool begin() override
    {
        return true;
    }

    bool read(MotionData& out) override
    {
        const float t = millis() / 1000.0f;
        const float pi = 3.14159265f;

        out.accelX = 0.4f * sinf(2.0f * pi * 0.7f * t);
        out.accelY = 0.3f * sinf(2.0f * pi * 1.1f * t);
        out.accelZ = 1.0f + 0.1f * sinf(2.0f * pi * 0.5f * t);

        out.gyroX = 15.0f * sinf(2.0f * pi * 0.6f * t);
        out.gyroY = 10.0f * sinf(2.0f * pi * 0.9f * t);
        out.gyroZ = 5.0f * sinf(2.0f * pi * 0.4f * t);

        out.sequence = 0;
        out.timestampMs = millis();

        return true;
    }

    uint32_t i2cErrorCount() const override
    {
        return 0;
    }
};

MockSensorImpl mockSensor;
} // namespace

MotionSensor* getSensor()
{
    return &mockSensor;
}