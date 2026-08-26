#include <Arduino.h>
#include <unity.h>
#include "MotionAlgorithms.h"

// Unity required test setup and teardown functions.
void setUp(void) {}
void tearDown(void) {}

// Test 1: Typical step should be detected.
void test_normal_step() {
    MotionData simulatedData = {0};
    simulatedData.accelX = 1.8f;
    simulatedData.accelZ = 9.8f; 
    TEST_ASSERT_TRUE(detectMovement(simulatedData)); 
}

// Test 2: Moonwalk-like low acceleration should not register as a step.
void test_moonwalk_is_not_a_step() {
    MotionData simulatedData = {0};
    simulatedData.accelX = -0.5f; 
    simulatedData.gyroZ = 120.0f;
    TEST_ASSERT_FALSE(detectMovement(simulatedData)); 
}

// Test 3: Free-fall state should not trigger a step.
void test_jump_in_the_air() {
    MotionData simulatedData = {0};
    simulatedData.accelX = 0.1f;
    simulatedData.accelZ = 0.1f; // Simulate free-fall acceleration.
    TEST_ASSERT_FALSE(detectMovement(simulatedData)); 
}

// Test 4: Very high acceleration should still be recognized as movement.
void test_very_fast_step() {
    MotionData simulatedData = {0};
    simulatedData.accelX = 4.5f; 
    TEST_ASSERT_TRUE(detectMovement(simulatedData)); 
}

// Test 5: Movement detection should work in all directions.
void test_omnidirectional_walking() {
    MotionData data = {0};

    // Step forward (positive X axis).
    data.accelX = 2.0f; data.accelY = 0.0f; data.accelZ = 1.0f;
    TEST_ASSERT_TRUE(detectMovement(data));

    // Step backward (negative X axis).
    data.accelX = -2.0f; data.accelY = 0.0f; data.accelZ = 1.0f;
    TEST_ASSERT_TRUE(detectMovement(data));

    // Step to the right (positive Y axis).
    data.accelX = 0.0f; data.accelY = 2.0f; data.accelZ = 1.0f;
    TEST_ASSERT_TRUE(detectMovement(data));

    // Step to the left (negative Y axis).
    data.accelX = 0.0f; data.accelY = -2.0f; data.accelZ = 1.0f;
    TEST_ASSERT_TRUE(detectMovement(data));

    // Diagonal step with combined axes.
    data.accelX = 1.5f; data.accelY = 1.5f; data.accelZ = 1.0f;
    TEST_ASSERT_TRUE(detectMovement(data));
}

// Test 6: Stress test with repeated step-like samples.
void test_stress_multiple_continuous_steps() {
    int detected_steps = 0;
    int expected_steps = 500;
    
    for (int i = 0; i < 50000; i++) {
        MotionData data = {0};
        if (i % 100 == 0) {
            data.accelX = 2.5f; // Step impulse.
        } else {
            data.accelX = 0.1f; // Low acceleration between steps.
        }
        
        if (detectMovement(data)) {
            detected_steps++;
        }
    }
    
    // Explicitly use _INT to prevent hex garbage output on Cortex-M4
    TEST_ASSERT_EQUAL_INT(expected_steps, detected_steps);
}

// ==========================================
// Main test runner.
// ==========================================

void setup() {
    delay(2000); 
    UNITY_BEGIN();
    
    RUN_TEST(test_normal_step);
    RUN_TEST(test_moonwalk_is_not_a_step);
    RUN_TEST(test_jump_in_the_air);
    RUN_TEST(test_very_fast_step);
    RUN_TEST(test_omnidirectional_walking);
    RUN_TEST(test_stress_multiple_continuous_steps);
    
    UNITY_END();
    delay(1000); // Wait for Serial buffer to flush before reset
}

void loop() {
    // Empty loop for Unity test execution.
}