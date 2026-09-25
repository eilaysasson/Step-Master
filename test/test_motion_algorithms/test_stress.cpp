// ========================================================================================================
// File: test_stress.cpp
// Purpose: Stress tests including Marathon simulations, sudden stops, complex multi-motion transitions, 
// and random vibration noise rejection over extended time periods.
// ========================================================================================================

#include <Arduino.h>
#include <unity.h>
#include "algorithem/MotionClassifier.h"
#include "filter/EmaFilter.hpp"
#include "detectors/StepMotion.hpp"
#include "detectors/JumpMotion.hpp"

// ----------------------------------------------------------------------------------------------------
// Helper Function: Stabilize Window
// ----------------------------------------------------------------------------------------------------
#include "TestHelpers.h"

// ----------------------------------------------------------------------------------------------------
// Test: Marathon Stress (5000 Steps)
// ----------------------------------------------------------------------------------------------------
void test_marathon_stress_5000_steps() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    classifier.addDetector(&stepDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    // Simulate 5000 continuous fast steps
    for (int i = 0; i < 5000; i++) {
        MotionData stepData = {0};
        stepData.accelX = 1.5f;
        stepData.accelZ = 1.0f;
        stepData.timestampMs = clockMs;
        
        classifier.update(stepData);
        clockMs += 400; // Clear debounce
    }

    TEST_ASSERT_EQUAL(5000, stepDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Jump Stress Marathon (50 Jumps)
// ----------------------------------------------------------------------------------------------------
void test_jump_stress_marathon() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    JumpMotion jumpDetector;
    classifier.addDetector(&jumpDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    for (int jump = 0; jump < 50; jump++) {
        MotionData data = {0};
        
        // Flight (250ms)
        data.accelZ = 0.15f; 
        for (int i = 0; i < 25; i++) {
            data.timestampMs = clockMs;
            classifier.update(data);
            clockMs += 10;
        }

        // Impact (2.8g)
        data.accelZ = 2.8f;
        for (int i = 0; i < 5; i++) {
            data.timestampMs = clockMs;
            classifier.update(data);
            clockMs += 10;
        }
        
        // Recovery
        data.accelZ = 1.0f;
        for (int i = 0; i < 30; i++) {
            data.timestampMs = clockMs;
            classifier.update(data);
            clockMs += 10;
        }
    }

    TEST_ASSERT_EQUAL(50, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Sudden Stop No False Jumps
// ----------------------------------------------------------------------------------------------------
void test_sudden_stop_no_false_jumps() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    JumpMotion jumpDetector;
    classifier.addDetector(&jumpDetector);
    
    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData data = {0};
    
    // Simulate high-impact running
    data.accelX = 2.0f;
    data.accelZ = 1.0f;
    for (int i = 0; i < 50; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }

    // Sudden complete stop to 1.0g
    data.accelX = 0.0f;
    data.accelZ = 1.0f;
    for (int i = 0; i < 100; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }

    TEST_ASSERT_EQUAL(0, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Test: Complex Transition (Walk -> Jump -> Walk)
// ----------------------------------------------------------------------------------------------------
void test_complex_transition_walk_jump_walk() {
    EmaFilter filter(0.53f);
    MotionClassifier classifier(&filter);
    StepMotion stepDetector;
    JumpMotion jumpDetector;
    
    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stepDetector);

    uint32_t clockMs = 0;
    stabilizeWindow(classifier, clockMs);

    MotionData data = {0};

    // Phase 1: Walk 2 steps
    data.accelZ = 1.8f;
    data.timestampMs = clockMs;
    classifier.update(data);
    clockMs += 500; 

    data.timestampMs = clockMs;
    classifier.update(data);
    clockMs += 500;

    // Phase 2: Jump over an obstacle
    data.accelZ = 0.1f; // Free fall
    for (int i = 0; i < 25; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }
    data.accelZ = 3.0f; // Landing
    for (int i = 0; i < 5; i++) {
        data.timestampMs = clockMs;
        classifier.update(data);
        clockMs += 10;
    }

    clockMs += 400; // Recovery pause after landing

    // Phase 3: Walk 2 more steps
    data.accelZ = 1.6f;
    data.timestampMs = clockMs;
    classifier.update(data);
    clockMs += 500;

    data.timestampMs = clockMs;
    classifier.update(data);

    TEST_ASSERT_EQUAL(4, stepDetector.getCount());
    TEST_ASSERT_EQUAL(1, jumpDetector.getCount());
}

// ----------------------------------------------------------------------------------------------------
// Unity Environment
// ----------------------------------------------------------------------------------------------------
void setUp(void) {}
void tearDown(void) {}

void setup() {
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_marathon_stress_5000_steps);
    RUN_TEST(test_jump_stress_marathon);
    RUN_TEST(test_sudden_stop_no_false_jumps);
    RUN_TEST(test_complex_transition_walk_jump_walk);
    UNITY_END();
}

void loop() {}