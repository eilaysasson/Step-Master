// ========================================================================================================
// File: test_stair.cpp
// Purpose: Validates mid-pitch kinematics (25 <= Pitch < 65) routing strictly to the Stair detector.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "algorithem/MotionClassifier.h"
#include "filter/EmaFilter.hpp"
#include "detectors/StairMotion.hpp"
#include "detectors/StepMotion.hpp"
#include "TestHelpers.h"

// ----------------------------------------------------------------------------------------------------
// Test: Valid Stair Climb (Pitch ~ 45 degrees)
// ----------------------------------------------------------------------------------------------------
void test_valid_stair_climb() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StairMotion stairDetector;
    classifier.addDetector(&stairDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    // ax=1.0, az=1.0 generates exactly 45 degrees of pitch. Fits perfectly in the 25-65 bounds.
    injectKinematicPeak(classifier, clockMs, 1.0f, 1.0f);

    TEST_ASSERT_EQUAL(1, stairDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: High Energy Flat Walk Rejected as Stairs
// ----------------------------------------------------------------------------------------------------
void test_high_energy_flat_walk_rejected_as_stairs() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StairMotion stairDetector;
    StepMotion stepDetector;
    classifier.addDetector(&stairDetector);
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    // Extreme impact (multiplier 3.5 = ~3.5g) but 0 degrees pitch. Should route to STEP.
    injectKinematicPeak(classifier, clockMs, 0.0f, 1.0f, 3.5f);

    TEST_ASSERT_EQUAL(0, stairDetector.getCount());
    TEST_ASSERT_EQUAL(1, stepDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
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