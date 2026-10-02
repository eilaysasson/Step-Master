// ========================================================================================================
// File: test_jump.cpp
// Purpose: Validates high-pitch kinematics (>= 65 degrees) for Knees-to-Chest jumps and High Kicks.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "config/AlgoConfig.hpp"
#include "algorithm/RuleBasedFSMClassifier.hpp"
#include "algorithm/FeatureExtractor.hpp"
#include "filter/EmaFilter.hpp"
#include "detectors/JumpMotion.hpp"
#include "detectors/StepMotion.hpp"
#include "TestHelpers.h"

void test_knees_to_chest_jump() {
    AlgoConfig config;
    EmaFilter filter(0.53f);
    FeatureExtractor extractor(&filter);
    RuleBasedFSMClassifier classifier;
    JumpMotion jumpDetector;
    
    classifier.addDetector(&jumpDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(extractor, clockMs);

    injectKinematicPeak(extractor, classifier, config, clockMs, 1.5f, 0.5f);

    TEST_ASSERT_EQUAL(1, jumpDetector.getCount());
}

void test_high_kick_triggers_jump() {
    AlgoConfig config;
    EmaFilter filter(0.53f);
    FeatureExtractor extractor(&filter);
    RuleBasedFSMClassifier classifier;
    JumpMotion jumpDetector;
    StepMotion stepDetector;
    
    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(extractor, clockMs);

    injectKinematicPeak(extractor, classifier, config, clockMs, 2.0f, 0.2f);

    TEST_ASSERT_EQUAL(1, jumpDetector.getCount());
    TEST_ASSERT_EQUAL(0, stepDetector.getCount());
}

void test_flat_hard_landing_is_not_jump() {
    AlgoConfig config;
    EmaFilter filter(0.53f);
    FeatureExtractor extractor(&filter);
    RuleBasedFSMClassifier classifier;
    JumpMotion jumpDetector;
    StepMotion stepDetector;
    
    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(extractor, clockMs);

    injectKinematicPeak(extractor, classifier, config, clockMs, 0.0f, 1.0f, 4.0f);

    TEST_ASSERT_EQUAL(0, jumpDetector.getCount());
    TEST_ASSERT_EQUAL(1, stepDetector.getCount());
}

void setUp(void) {}
void tearDown(void) {}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_knees_to_chest_jump);
    RUN_TEST(test_high_kick_triggers_jump);
    RUN_TEST(test_flat_hard_landing_is_not_jump);
    UNITY_END();
}

void loop() {}