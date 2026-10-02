// ========================================================================================================
// File: TestHelpers.h
// Purpose: Provides robust deterministic signal injection to perfectly test the Pitch-Based FSM logic.
// Note: Adapted for the decoupled IClassifier architecture.
// ========================================================================================================

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "algorithm/RuleBasedFSMClassifier.hpp"
#include "algorithm/FeatureExtractor.hpp"
#include "config/AlgoConfig.hpp"
#include "sensors/MotionSensor.h"
#include "sensors/MotionFeatures.h"

inline void stabilizeWindow(FeatureExtractor& extractor, uint32_t& clockMs) {
    MotionData idleData = {0};
    idleData.accelZ = 1.0f;
    for (int i = 0; i < 100; i++) {
        idleData.timestampMs = clockMs;
        extractor.process(idleData); 
        clockMs += 10;
    }
}

inline void injectKinematicPeak(FeatureExtractor& extractor, RuleBasedFSMClassifier& classifier, const AlgoConfig& config, uint32_t& clockMs, float baseAx, float baseAz, float peakMultiplier = 2.0f) {
    MotionData data = {0};
    
    data.accelX = baseAx;
    data.accelZ = baseAz;
    for (int i = 0; i < 100; i++) {
        data.timestampMs = clockMs;
        MotionFeatures features = extractor.process(data);
        classifier.evaluate(features, config);
        clockMs += 10;
    }

    data.accelX = baseAx * peakMultiplier;
    data.accelZ = baseAz * peakMultiplier;
    for (int i = 0; i < 2; i++) {
        data.timestampMs = clockMs;
        MotionFeatures features = extractor.process(data);
        classifier.evaluate(features, config);
        clockMs += 10;
    }

    data.accelX = baseAx;
    data.accelZ = baseAz;
    for (int i = 0; i < 5; i++) {
        data.timestampMs = clockMs;
        MotionFeatures features = extractor.process(data);
        classifier.evaluate(features, config);
        clockMs += 10;
    }

    data.accelX = 0.0f;
    data.accelZ = 1.0f;
    for (int i = 0; i < 30; i++) {
        data.timestampMs = clockMs;
        MotionFeatures features = extractor.process(data);
        classifier.evaluate(features, config);
        clockMs += 10;
    }
}

#endif // TEST_HELPERS_H