#include "algorithem/MotionClassifier.h"
#include "params/AlgoParams.hpp"
#include <cmath>

MotionClassifier::MotionClassifier(IFilter* filter) 
    : magnitudeFilter_(filter), detectorCount_(0), currentPitch_(0.0f) {
}

void MotionClassifier::addDetector(BaseMotion* detector) {
    if (detectorCount_ < MAX_DETECTORS) {
        detectors_[detectorCount_] = detector;
        detectorCount_++;
    }
}

MotionState MotionClassifier::update(const MotionData& rawData) {
    float rawMag = std::sqrt((rawData.accelX * rawData.accelX) + 
                             (rawData.accelY * rawData.accelY) + 
                             (rawData.accelZ * rawData.accelZ));

    float smoothMag = magnitudeFilter_->process(rawMag);

    // Calculate kinematic pitch using a Complementary Filter (96% Gyro, 4% Accel)
    float dtSeconds = AlgoParams::SAMPLE_INTERVAL_US / 1000000.0f;
    float accelPitch = std::atan2(rawData.accelX, std::sqrt((rawData.accelY * rawData.accelY) + (rawData.accelZ * rawData.accelZ))) * 57.2958f;
    currentPitch_ = 0.96f * (currentPitch_ + rawData.gyroY * dtSeconds) + 0.04f * accelPitch;

    slidingWindow_.push(smoothMag);

    MotionFeatures features = {rawMag, smoothMag, currentPitch_, rawData.timestampMs};

    for (uint8_t i = 0; i < detectorCount_; i++) {
        MotionState evaluatedState = detectors_[i]->evaluate(features, slidingWindow_);
        if (evaluatedState != MotionState::IDLE && evaluatedState != MotionState::NOISE) {
            return evaluatedState;
        }
    }

    return MotionState::IDLE;
}