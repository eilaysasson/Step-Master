// ========================================================================================================
// File: test_stair.cpp
// Purpose: Validates mid-pitch kinematics routing strictly to the Stair detector.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "config/AlgoConfig.hpp"
#include "algorithm/RuleBasedFSMClassifier.hpp"
#include "algorithm/FeatureExtractor.hpp"
#include "filter/EmaFilter.hpp"
#include "detectors/StairMotion.hpp"
#include "detectors/StepMotion.hpp"
#include "TestHelpers.h"

void test_valid_stair_climb() {
    AlgoConfig config;
    EmaFilter filter(0.53f);
    FeatureExtractor extractor(&filter);
    RuleBasedFSMClassifier classifier;
    StairMotion stairDetector;
    
    classifier.addDetector(&stairDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(extractor, clockMs);

    injectKinematicPeak(extractor, classifier, config, clockMs, 1.0f, 1.0f);

    TEST_ASSERT_EQUAL(1, stairDetector.getCount());
}

void test_high_energy_flat_walk_rejected_as_stairs() {
    AlgoConfig config;
    EmaFilter filter(0.53f);
    FeatureExtractor extractor(&filter);
    RuleBasedFSMClassifier classifier;
    StairMotion stairDetector;
    StepMotion stepDetector;
    
    classifier.addDetector(&stairDetector);
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(extractor, clockMs);

    injectKinematicPeak(extractor, classifier, config, clockMs, 0.0f, 1.0f, 3.5f);

    TEST_ASSERT_EQUAL(0, stairDetector.getCount());
    TEST_ASSERT_EQUAL(1, stepDetector.getCount());
}

void setUp(void) {}
void tearDown(void) {}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_valid_stair_climb);
    RUN_TEST(test_high_energy_flat_walk_rejected_as_stairs);
    UNITY_END();
}

void loop() {}