#include "MotionAlgorithms.h"
#include <cmath>

float calculateAccelerationMagnitude(MotionData data)
{
    return std::sqrt(data.accelX * data.accelX +
                     data.accelY * data.accelY +
                     data.accelZ * data.accelZ);
}

bool detectMovement(MotionData data)
{
    // Compute the vector magnitude of acceleration.
    float mag = calculateAccelerationMagnitude(data);

    // Movement detection threshold in g units.
    constexpr float STEP_THRESHOLD_G = 1.5f;

    return (mag > STEP_THRESHOLD_G);
}

void processMotion(MotionData data)
{
    // Placeholder for future motion processing logic.
    (void)data;
}