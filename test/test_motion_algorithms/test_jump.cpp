// ========================================================================================================
// File: test_jump.cpp
// Purpose: Validates high-pitch kinematics (>= 65 degrees) for Knees-to-Chest jumps and High Kicks.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "algorithem/MotionClassifier.h"
#include "filter/EmaFilter.hpp"
#include "detectors/JumpMotion.hpp"
#include "detectors/StepMotion.hpp"
#include "TestHelpers.h"

// ----------------------------------------------------------------------------------------------------
// Test: Standard Knees-to-Chest Jump (Pitch ~ 71.5 degrees)
// ----------------------------------------------------------------------------------------------------
void test_knees_to_chest_jump() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    JumpMotion jumpDetector;
    classifier.addDetector(&jumpDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    // ax=1.5, az=0.5 generates ~71.5 degrees of pitch. Safely above the 65.0 boundary.
    injectKinematicPeak(classifier, clockMs, 1.5f, 0.5f);

    TEST_ASSERT_EQUAL(1, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Ballet / Soccer High Kick (Pitch ~ 84 degrees)
// ----------------------------------------------------------------------------------------------------
void test_high_kick_triggers_jump() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    JumpMotion jumpDetector;
    StepMotion stepDetector;
    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    // ax=2.0, az=0.2 generates ~84 degrees of extreme pitch rotation.
    injectKinematicPeak(classifier, clockMs, 2.0f, 0.2f);

    TEST_ASSERT_EQUAL(1, jumpDetector.getCount());
    TEST_ASSERT_EQUAL(0, stepDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Hard Flat Landing Rejected as Jump (Pitch 0)
// ----------------------------------------------------------------------------------------------------
void test_flat_hard_landing_is_not_jump() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    JumpMotion jumpDetector;
    StepMotion stepDetector;
    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    // Massive impact (multiplier 4.0 = ~4.0g) but leg is entirely flat (pitch 0).
    injectKinematicPeak(classifier, clockMs, 0.0f, 1.0f, 4.0f);

    TEST_ASSERT_EQUAL(0, jumpDetector.getCount());
    TEST_ASSERT_EQUAL(1, stepDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
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