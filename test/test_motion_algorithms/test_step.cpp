// ========================================================================================================
// File: test_step.cpp
// Purpose: Validates step detection logic (walking/running) using low-pitch kinematics.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "config/AlgoConfig.hpp"
#include "algorithm/RuleBasedFSMClassifier.hpp"
#include "algorithm/FeatureExtractor.hpp"
#include "filter/EmaFilter.hpp"
#include "detectors/StepMotion.hpp"
#include "detectors/JumpMotion.hpp"
#include "TestHelpers.h"

void test_normal_walking_step() {
    AlgoConfig config;
    EmaFilter filter(0.53f);
    FeatureExtractor extractor(&filter);
    RuleBasedFSMClassifier classifier;
    StepMotion stepDetector;
    
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(extractor, clockMs);

    injectKinematicPeak(extractor, classifier, config, clockMs, 0.0f, 1.0f);

    TEST_ASSERT_EQUAL(1, stepDetector.getCount());
}

void test_running_is_counted_as_steps() {
    AlgoConfig config;
    EmaFilter filter(0.53f);
    FeatureExtractor extractor(&filter);
    RuleBasedFSMClassifier classifier;
    StepMotion stepDetector;
    JumpMotion jumpDetector;
    
    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(extractor, clockMs); 

    for (int step = 0; step < 4; step++) {
        injectKinematicPeak(extractor, classifier, config, clockMs, 0.3f, 1.0f, 2.5f);
    }

    TEST_ASSERT_EQUAL(4, stepDetector.getCount());
    TEST_ASSERT_EQUAL(0, jumpDetector.getCount());
}

void test_debounce_time_lock_rejection() {
    AlgoConfig config;
    EmaFilter filter(0.53f);
    FeatureExtractor extractor(&filter);
    RuleBasedFSMClassifier classifier;
    StepMotion stepDetector;
    
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(extractor, clockMs);

    MotionData data = {0};
    
    data.accelZ = 2.0f; data.timestampMs = clockMs; 
    classifier.evaluate(extractor.process(data), config);
    
    data.accelZ = 1.0f; data.timestampMs = clockMs + 10; 
    classifier.evaluate(extractor.process(data), config);
    
    data.accelZ = 2.0f; data.timestampMs = clockMs + 100; 
    classifier.evaluate(extractor.process(data), config);
    
    data.accelZ = 1.0f; data.timestampMs = clockMs + 110; 
    classifier.evaluate(extractor.process(data), config);

    TEST_ASSERT_EQUAL(1, stepDetector.getCount());
}

void setUp(void) {}
void tearDown(void) {}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_normal_walking_step);
    RUN_TEST(test_running_is_counted_as_steps);
    RUN_TEST(test_debounce_time_lock_rejection);
    UNITY_END();
}

void loop() {}