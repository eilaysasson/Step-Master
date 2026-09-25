#include <Arduino.h>
#include <unity.h>
#include "sensors/MockSensor.h"

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
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_mock_sensor_initializes_and_reads);
    UNITY_END();
}

void loop() {}