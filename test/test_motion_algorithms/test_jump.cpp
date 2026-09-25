// ========================================================================================================
// File: test_jump.cpp
// Purpose: Extensively tests the Airborne Free-Fall -> Landing Impact FSM including extreme edge cases,
// high jumps, long jumps, and rope skipping variations.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "algorithm/MotionClassifier.h"
#include "filter/EmaFilter.hpp"
#include "JumpMotion.hpp"
#include "StepMotion.hpp"

// ----------------------------------------------------------------------------------------------------
// Helper Function: Stabilize Window
// ----------------------------------------------------------------------------------------------------
#include "TestHelpers.h"

// ----------------------------------------------------------------------------------------------------
// Test: Flight Too Short Rejected
// ----------------------------------------------------------------------------------------------------
void test_flight_too_short_rejected() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    JumpMotion jumpDetector;
    classifier.addDetector(&jumpDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData data = {0};
    data.accelZ = 0.1f; // Free fall (< 0.35g)
    
    // Only 100ms of flight (minimum requirement is 180ms)
    for (int i = 0; i < 10; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }

    data.accelZ = 2.0f; // Impact
    MotionState result = MotionState::IDLE;
    for (int i = 0; i < 5; i++) {
        data.timestampMs = clockMs;
        MotionState frameState = classifier.update(data);
        if (frameState == MotionState::JUMP) {
            result = frameState;
        }
        clockMs += 10;
    }

    TEST_ASSERT_NOT_EQUAL(MotionState::JUMP, result);
    TEST_ASSERT_EQUAL(0, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Soft Landing Jump
// ----------------------------------------------------------------------------------------------------
void test_soft_landing_jump() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    JumpMotion jumpDetector;
    classifier.addDetector(&jumpDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData data = {0};
    data.accelZ = 0.1f; 
    
    // 300ms of valid flight
    for (int i = 0; i < 30; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }

    // Soft landing impact (0.8g), crosses the 0.7g threshold but nowhere near 3.0g
    data.accelZ = 0.8f;
    MotionState result = MotionState::IDLE;
    for (int i = 0; i < 5; i++) {
        data.timestampMs = clockMs;
        MotionState frameState = classifier.update(data);
        if (frameState == MotionState::JUMP) {
            result = frameState;
        }
        clockMs += 10;
    }

    TEST_ASSERT_EQUAL(MotionState::JUMP, result);
    TEST_ASSERT_EQUAL(1, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: High Jump (Hard Landing)
// ----------------------------------------------------------------------------------------------------
void test_high_jump_hard_landing() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    JumpMotion jumpDetector;
    classifier.addDetector(&jumpDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData data = {0};
    
    // Flight (400ms)
    data.accelZ = 0.1f; 
    for (int i = 0; i < 40; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }

    // Hard Impact (4.0g - Extreme force)
    data.accelZ = 4.0f;
    MotionState result = MotionState::IDLE;
    for (int i = 0; i < 5; i++) {
        data.timestampMs = clockMs;
        MotionState frameState = classifier.update(data);
        if (frameState == MotionState::JUMP) {
            result = frameState;
        }
        clockMs += 10;
    }

    TEST_ASSERT_EQUAL(MotionState::JUMP, result);
    TEST_ASSERT_EQUAL(1, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Long Jump (Soft Landing)
// ----------------------------------------------------------------------------------------------------
void test_long_jump_soft_landing() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    JumpMotion jumpDetector;
    classifier.addDetector(&jumpDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData data = {0};
    
    // Flight (700ms - Long horizontal jump)
    data.accelZ = 0.2f; 
    for (int i = 0; i < 70; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }

    // Softer horizontal impact (2.0g)
    data.accelZ = 2.0f;
    MotionState result = MotionState::IDLE;
    for (int i = 0; i < 5; i++) {
        data.timestampMs = clockMs;
        MotionState frameState = classifier.update(data);
        if (frameState == MotionState::JUMP) {
            result = frameState;
        }
        clockMs += 10;
    }

    TEST_ASSERT_EQUAL(MotionState::JUMP, result);
    TEST_ASSERT_EQUAL(1, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Rope Skipping Counted as Steps Not Jumps
// ----------------------------------------------------------------------------------------------------
void test_skipping_counted_as_steps_not_jumps() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    JumpMotion jumpDetector;
    
    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stepDetector); 
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    // Simulate 10 realistic rope skips 
    for (int skip = 0; skip < 10; skip++) {
        MotionData data = {0};
        
        // Short flight (~150ms) - Safely below the 180ms Jump minimum
        data.accelZ = 0.2f; 
        for (int i = 0; i < 15; i++) {
            data.timestampMs = clockMs;
            classifier.update(data);
            clockMs += 10;
        }

        // Quick Impact (~50ms at 2.5g)
        data.accelZ = 2.5f;
        for (int i = 0; i < 5; i++) {
            data.timestampMs = clockMs;
            classifier.update(data);
            clockMs += 10;
        }
        
        // Rest phase on the ground (~600ms)
        // This is crucial: It allows the 1-second O(1) Sliding Window to flush out 
        // the extreme variance, allowing the dynamic threshold to reset for the next step.
        data.accelZ = 1.0f;
        for (int i = 0; i < 60; i++) {
            data.timestampMs = clockMs;
            classifier.update(data);
            clockMs += 10;
        }
    }

    // Verify 10 skips counted entirely as Steps, 0 Jumps detected.
    TEST_ASSERT_EQUAL(10, stepDetector.getCount());
    TEST_ASSERT_EQUAL(0, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Unity Environment
// ----------------------------------------------------------------------------------------------------
void setUp(void) {}
void tearDown(void) {}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_flight_too_short_rejected);
    RUN_TEST(test_soft_landing_jump);
    RUN_TEST(test_high_jump_hard_landing);
    RUN_TEST(test_long_jump_soft_landing);
    RUN_TEST(test_skipping_counted_as_steps_not_jumps);
    UNITY_END();
}

void loop() {}