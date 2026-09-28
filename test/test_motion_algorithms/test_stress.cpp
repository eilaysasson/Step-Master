// ========================================================================================================
// File: test_stress.cpp
// Purpose: Validates kinematic mutual exclusion and transitions over long sequences and combinations.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "algorithem/MotionClassifier.h"
#include "filter/EmaFilter.hpp"
#include "detectors/StepMotion.hpp"
#include "detectors/JumpMotion.hpp"
#include "detectors/StairMotion.hpp"
#include "TestHelpers.h"

// ----------------------------------------------------------------------------------------------------
// Test: Marathon Stress (1000 Steps)
// ----------------------------------------------------------------------------------------------------
void test_marathon_stress_1000_steps() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    for (int i = 0; i < 1000; i++) {
        injectKinematicPeak(classifier, clockMs, 0.0f, 1.0f);
    }

    TEST_ASSERT_EQUAL(1000, stepDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Complex Triathlon (Walk -> Stairs -> Jump)
// ----------------------------------------------------------------------------------------------------
void test_complex_triathlon_transitions() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    JumpMotion jumpDetector;
    StairMotion stairDetector;
    
    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stairDetector);
    classifier.addDetector(&stepDetector);

    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    // 2 Steps (Pitch 0)
    injectKinematicPeak(classifier, clockMs, 0.0f, 1.0f);
    injectKinematicPeak(classifier, clockMs, 0.0f, 1.0f);
    
    // 3 Stairs (Pitch 45)
    injectKinematicPeak(classifier, clockMs, 1.0f, 1.0f);
    injectKinematicPeak(classifier, clockMs, 1.0f, 1.0f);
    injectKinematicPeak(classifier, clockMs, 1.0f, 1.0f);

    // 1 Jump (Pitch 76)
    injectKinematicPeak(classifier, clockMs, 2.0f, 0.5f);

    TEST_ASSERT_EQUAL(2, stepDetector.getCount());
    TEST_ASSERT_EQUAL(3, stairDetector.getCount());
    TEST_ASSERT_EQUAL(1, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
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