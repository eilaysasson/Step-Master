// ========================================================================================================
// File: test_stress.cpp
// Purpose: Validates kinematic mutual exclusion and transitions over long sequences and combinations.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "config/AlgoConfig.hpp"
#include "algorithm/RuleBasedFSMClassifier.hpp"
#include "algorithm/FeatureExtractor.hpp"
#include "filter/EmaFilter.hpp"
#include "detectors/StepMotion.hpp"
#include "detectors/JumpMotion.hpp"
#include "detectors/StairMotion.hpp"
#include "TestHelpers.h"

void test_marathon_stress_1000_steps() {
    AlgoConfig config;
    EmaFilter filter(0.53f);
    FeatureExtractor extractor(&filter);
    RuleBasedFSMClassifier classifier;
    StepMotion stepDetector;
    
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(extractor, clockMs);

    for (int i = 0; i < 1000; i++) {
        injectKinematicPeak(extractor, classifier, config, clockMs, 0.0f, 1.0f);
    }

    TEST_ASSERT_EQUAL(1000, stepDetector.getCount());
}

void test_complex_triathlon_transitions() {
    AlgoConfig config;
    EmaFilter filter(0.53f);
    FeatureExtractor extractor(&filter);
    RuleBasedFSMClassifier classifier;
    StepMotion stepDetector;
    JumpMotion jumpDetector;
    StairMotion stairDetector;
    
    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stairDetector);
    classifier.addDetector(&stepDetector);

    uint32_t clockMs = 0;
    stabilizeWindow(extractor, clockMs);

    injectKinematicPeak(extractor, classifier, config, clockMs, 0.0f, 1.0f);
    injectKinematicPeak(extractor, classifier, config, clockMs, 0.0f, 1.0f);
    
    injectKinematicPeak(extractor, classifier, config, clockMs, 1.0f, 1.0f);
    injectKinematicPeak(extractor, classifier, config, clockMs, 1.0f, 1.0f);
    injectKinematicPeak(extractor, classifier, config, clockMs, 1.0f, 1.0f);

    injectKinematicPeak(extractor, classifier, config, clockMs, 2.0f, 0.5f);

    TEST_ASSERT_EQUAL(2, stepDetector.getCount());
    TEST_ASSERT_EQUAL(3, stairDetector.getCount());
    TEST_ASSERT_EQUAL(1, jumpDetector.getCount());
}

void setUp(void) {}
void tearDown(void) {}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_marathon_stress_1000_steps);
    RUN_TEST(test_complex_triathlon_transitions);
    UNITY_END();
}

void loop() {}