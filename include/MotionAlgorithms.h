#ifndef MOTION_ALGORITHMS_H
#define MOTION_ALGORITHMS_H

#include "MotionSensor.h"

float calculateAccelerationMagnitude(MotionData data);

bool detectMovement(MotionData data);

void processMotion(MotionData data);

#endif
