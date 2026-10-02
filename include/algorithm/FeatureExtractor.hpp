// ========================================================================================================
// File: FeatureExtractor.hpp
// Purpose: Dedicated DSP pipeline executing filtering, windowing, and peak detection prior to classification.
// ========================================================================================================

#ifndef FEATURE_EXTRACTOR_HPP
#define FEATURE_EXTRACTOR_HPP

#include "sensors/MotionSensor.h"
#include "sensors/MotionFeatures.h"
#include "filter/IFilter.hpp"
#include "utils/SlidingWindow.hpp"
#include "utils/PeakDetector.hpp"
#include "params/AlgoParams.hpp"
#include <cmath>

class FeatureExtractor {
public:
    FeatureExtractor(IFilter* filter) : magnitudeFilter_(filter), currentPitch_(0.0f) {}

    MotionFeatures process(const MotionData& rawData) {
        float rawMag = std::sqrt((rawData.accelX * rawData.accelX) + 
                                 (rawData.accelY * rawData.accelY) + 
                                 (rawData.accelZ * rawData.accelZ));

        float smoothMag = magnitudeFilter_->process(rawMag);

        float dtSeconds = AlgoParams::SAMPLE_INTERVAL_US / 1000000.0f;
        float accelPitch = std::atan2(rawData.accelX, std::sqrt((rawData.accelY * rawData.accelY) + (rawData.accelZ * rawData.accelZ))) * 57.2958f;
        
        // Removed Magic Numbers: Now using parameters from AlgoParams
        currentPitch_ = AlgoParams::PITCH_GYRO_WEIGHT * (currentPitch_ + rawData.gyroY * dtSeconds) + AlgoParams::PITCH_ACCEL_WEIGHT * accelPitch;

        slidingWindow_.push(smoothMag);
        
        float currentPeakValue = 0.0f;
        bool currentIsPeak = peakDetector_.update(smoothMag, currentPeakValue);

        return {
            rawMag, smoothMag, currentPitch_, rawData.timestampMs,
            slidingWindow_.getMean(), slidingWindow_.getStdDev(),
            currentIsPeak, currentPeakValue
        };
    }

private:
    IFilter* magnitudeFilter_;
    SlidingWindow slidingWindow_;
    PeakDetector peakDetector_;
    float currentPitch_;
};

#endif // FEATURE_EXTRACTOR_HPP