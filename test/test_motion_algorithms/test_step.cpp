// ========================================================================================================
// File: test_step.cpp
// Purpose: Unit tests dedicated to Step detection logic, adaptive thresholds, noise rejection, 
// and dynamic state transitions (e.g., distinguishing running/skipping from jumping).
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "MotionClassifier.h"
#include "EmaFilter.hpp"
#include "StepMotion.hpp"
#include "JumpMotion.hpp"

// ----------------------------------------------------------------------------------------------------
// Helper Function: Stabilize Window
// ----------------------------------------------------------------------------------------------------
#include "TestHelpers.h"

// ----------------------------------------------------------------------------------------------------
// Test: Weak Slow Step
// ----------------------------------------------------------------------------------------------------
void test_weak_slow_step() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData stepData = {0};
    stepData.accelX = 0.6f; 
    stepData.accelZ = 1.0f; 
    stepData.timestampMs = clockMs + 100;

    MotionState result = classifier.update(stepData);
    TEST_ASSERT_EQUAL(MotionState::STEP, result);
    TEST_ASSERT_EQUAL(1, stepDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Debounce Double Impact Rejection
// ----------------------------------------------------------------------------------------------------
void test_debounce_double_impact_rejection() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData stepData = {0};
    stepData.accelX = 1.5f; 
    stepData.accelZ = 1.0f;

    stepData.timestampMs = clockMs + 100;
    classifier.update(stepData);

    // Second impact only 50ms later falls inside the refractory period
    stepData.timestampMs = clockMs + 150;
    classifier.update(stepData);

    TEST_ASSERT_EQUAL(1, stepDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Running is Counted as Steps, Not Jumps
// ----------------------------------------------------------------------------------------------------
void test_running_is_counted_as_steps_not_jumps() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    JumpMotion jumpDetector;
    
    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs); 

    // Simulate 4 fast running steps
    for (int step = 0; step < 4; step++) {
        MotionData data = {0};
        
        // Flight phase of running (short duration, ~100ms)
        data.accelZ = 0.4f; 
        for (int i = 0; i < 10; i++) {
            data.timestampMs = clockMs;
            classifier.update(data);
            clockMs += 10;
        }

        // Hard impact phase of running (high G force)
        data.accelZ = 2.5f;
        for (int i = 0; i < 5; i++) {
            data.timestampMs = clockMs;
            classifier.update(data);
            clockMs += 10;
        }
        
        clockMs += 100; // Wait out the step debounce timer
    }

    TEST_ASSERT_EQUAL(4, stepDetector.getCount());
    TEST_ASSERT_EQUAL(0, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Sitting Foot Tapping Rejection
// ----------------------------------------------------------------------------------------------------
void test_sitting_foot_tapping_rejection() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData data = {0};

    // Simulate 3 seconds of rapid, low-amplitude foot tapping
    for (int i = 0; i < 300; i++) {
        // Sine wave approximation between 0.9g and 1.1g
        data.accelZ = 1.0f + (0.1f * std::sin(i * 0.5f)); 
        data.timestampMs = clockMs;
        
        classifier.update(data);
        clockMs += 10;
    }

    TEST_ASSERT_EQUAL(0, stepDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Unity Environment
// ----------------------------------------------------------------------------------------------------
void setUp(void) {}
void tearDown(void) {}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_weak_slow_step);
    RUN_TEST(test_debounce_double_impact_rejection);
    RUN_TEST(test_running_is_counted_as_steps_not_jumps);
    RUN_TEST(test_sitting_foot_tapping_rejection);
    UNITY_END();
}

void loop() {}