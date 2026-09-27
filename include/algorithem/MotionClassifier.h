#ifndef MOTION_CLASSIFIER_H
#define MOTION_CLASSIFIER_H

#include <cstdint>
#include "sensors/MotionSensor.h"
#include "params/MotionState.h"
#include "sensors/MotionFeatures.h"
#include "filter/IFilter.hpp"
#include "utils/SlidingWindow.hpp"
#include "detectors/BaseMotion.hpp"

class MotionClassifier {
public:
    MotionClassifier(IFilter* filter);
    void addDetector(BaseMotion* detector);
    MotionState update(const MotionData& rawData);

private:
    IFilter* magnitudeFilter_;
    SlidingWindow slidingWindow_;
    
    static const uint8_t MAX_DETECTORS = 5;
    BaseMotion* detectors_[MAX_DETECTORS];
    uint8_t detectorCount_;
    
    float currentPitch_;
};

#endif // MOTION_CLASSIFIER_H