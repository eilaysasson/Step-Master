// ========================================================================================================
// File: test_stair.cpp
// Purpose: Validates the combined short-time energy and extreme pitch thresholds for stair climbing, 
// and ensures proper state transitions between stair climbing and flat walking.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "algorithem/MotionClassifier.h"
#include "filter/EmaFilter.hpp"
#include "detectors/StairMotion.hpp"
#include "detectors/StepMotion.hpp"

// ----------------------------------------------------------------------------------------------------
// Helper Function: Stabilize Window
// ----------------------------------------------------------------------------------------------------
#include "TestHelpers.h"

// ----------------------------------------------------------------------------------------------------
// Test: High Energy Flat Walk Rejected
// ----------------------------------------------------------------------------------------------------
void test_high_energy_flat_walk_rejected() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StairMotion stairDetector;
    classifier.addDetector(&stairDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData data = {0};
    data.accelX = 0.0f; // Zero pitch
    data.accelZ = 2.5f; // High vertical energy

    MotionState result = MotionState::IDLE;
    for (int i = 0; i < 10; i++) {
        data.timestampMs = clockMs;
        MotionState frameState = classifier.update(data);
        if (frameState == MotionState::STAIRS) {
            result = frameState;
        }
        clockMs += 10;
    }

    TEST_ASSERT_NOT_EQUAL(MotionState::STAIRS, result);
    TEST_ASSERT_EQUAL(0, stairDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Stairs to Flat Walking Transition
// ----------------------------------------------------------------------------------------------------
void test_stairs_to_flat_walking_transition() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    StairMotion stairDetector;
    
    classifier.addDetector(&stairDetector);
    classifier.addDetector(&stepDetector);

    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData data = {0};

    // Phase 1: Climb 2 stairs (High energy + high pitch)
    data.accelX = 1.5f; 
    data.accelZ = 1.5f; 
    
    for (int step = 0; step < 2; step++) {
        for (int i = 0; i < 5; i++) {
            data.timestampMs = clockMs;
            classifier.update(data);
            clockMs += 10;
        }
        clockMs += 400; // Delay between stairs
    }

    // Phase 2: Reach the top, start flat walking
    data.accelX = 0.0f; // Pitch ~0
    data.accelZ = 1.5f; // Purely vertical impact

    for (int step = 0; step < 3; step++) {
        for (int i = 0; i < 5; i++) {
            data.timestampMs = clockMs;
            classifier.update(data);
            clockMs += 10;
        }
        clockMs += 400;
    }

    TEST_ASSERT_EQUAL(2, stairDetector.getCount());
    TEST_ASSERT_EQUAL(3, stepDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Unity Environment
// ----------------------------------------------------------------------------------------------------
void setUp(void) {}
void tearDown(void) {}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_high_energy_flat_walk_rejected);
    RUN_TEST(test_stairs_to_flat_walking_transition);
    UNITY_END();
}

void loop() {}