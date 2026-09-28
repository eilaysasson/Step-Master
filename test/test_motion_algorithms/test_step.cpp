// ========================================================================================================
// File: test_step.cpp
// Purpose: Validates step detection logic (walking/running) using low-pitch kinematics (< 25 degrees).
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "algorithem/MotionClassifier.h"
#include "filter/EmaFilter.hpp"
#include "detectors/StepMotion.hpp"
#include "detectors/JumpMotion.hpp"
#include "TestHelpers.h"

// ----------------------------------------------------------------------------------------------------
// Test: Normal Walking Step (Pitch ~ 0 degrees)
// ----------------------------------------------------------------------------------------------------
void test_normal_walking_step() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    // ax=0.0, az=1.0 generates exactly 0 degrees of pitch. 
    injectKinematicPeak(classifier, clockMs, 0.0f, 1.0f);

    TEST_ASSERT_EQUAL(1, stepDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Running (Pitch ~ 15 degrees)
// ----------------------------------------------------------------------------------------------------
void test_running_is_counted_as_steps() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    JumpMotion jumpDetector;
    
    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs); 

    // 4 fast running steps: ax=0.3, az=1.0 yields ~16.7 degrees (Well below the 25 degree threshold)
    for (int step = 0; step < 4; step++) {
        injectKinematicPeak(classifier, clockMs, 0.3f, 1.0f, 2.5f);
    }

    TEST_ASSERT_EQUAL(4, stepDetector.getCount());
    TEST_ASSERT_EQUAL(0, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Debounce Time Lock Rejection
// ----------------------------------------------------------------------------------------------------
void test_debounce_time_lock_rejection() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData data = {0};
    
    // First valid peak
    data.accelZ = 2.0f; data.timestampMs = clockMs; classifier.update(data);
    data.accelZ = 1.0f; data.timestampMs = clockMs + 10; classifier.update(data);
    
    // Second peak falling perfectly inside the 250ms refractory window (at +100ms)
    data.accelZ = 2.0f; data.timestampMs = clockMs + 100; classifier.update(data);
    data.accelZ = 1.0f; data.timestampMs = clockMs + 110; classifier.update(data);

    // Only one should register!
    TEST_ASSERT_EQUAL(1, stepDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
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