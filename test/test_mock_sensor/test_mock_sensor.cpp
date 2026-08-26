#include <Arduino.h>
#include <unity.h>
#include "MockSensor.h"
#include "MotionAlgorithms.h"

void setUp(void) {}
void tearDown(void) {}

void test_mock_sensor_initializes_and_reads(void)
{
    MotionSensor* sensor = getSensor();
    TEST_ASSERT_NOT_NULL_MESSAGE(sensor, "Mock sensor instance should not be null.");

    bool started = sensor->begin();
    TEST_ASSERT_TRUE_MESSAGE(started, "Mock sensor should initialize successfully.");

    MotionData motion{};
    bool readOk = sensor->read(motion);
    TEST_ASSERT_TRUE_MESSAGE(readOk, "Mock sensor should return data successfully.");

    TEST_ASSERT_TRUE_MESSAGE(motion.timestampMs > 0, "Mock motion sample should include a valid timestamp.");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, motion.sequence, "Mock sensor sequence should start at 0.");
}

void test_mock_sensor_motion_detection_integration(void)
{
    MotionSensor* sensor = getSensor();
    TEST_ASSERT_TRUE(sensor->begin());

    MotionData motion{};
    TEST_ASSERT_TRUE(sensor->read(motion));

    // The mock sensor generates a small periodic acceleration waveform.
    // Movement detection should operate on the returned sample without crashing.
    bool movementDetected = detectMovement(motion);
    TEST_ASSERT_FALSE_MESSAGE(movementDetected, "The provided mock sample should not always trigger step detection.");
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();

    RUN_TEST(test_mock_sensor_initializes_and_reads);
    RUN_TEST(test_mock_sensor_motion_detection_integration);

    UNITY_END();
}

void loop() {}
