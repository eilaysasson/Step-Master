#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

struct MotionData
{
    float accelX;
    float accelY;
    float accelZ;

    float gyroX;
    float gyroY;
    float gyroZ;

    bool isStep;
};


class MotionSensor
{
public:

    virtual void begin() = 0;

    virtual MotionData read() = 0;
};


#endif