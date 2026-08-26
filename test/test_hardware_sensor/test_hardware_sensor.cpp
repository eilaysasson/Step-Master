#include <Arduino.h>
#include <unity.h>
#include "drivers/Mpu6050Sensor.h" 

Mpu6050Sensor sensor;

void setUp(void) {
    // Ensure the sensor is initialized before EVERY test runs.
    sensor.begin();
}

void tearDown(void) {
    // No per-test teardown required.
}

// Test 1: Verify sensor initialization and I2C connectivity.
void test_sensor_connection(void) {
    bool isConnected = sensor.begin();
    TEST_ASSERT_TRUE_MESSAGE(isConnected, "Sensor init failed! Check I2C wiring (SDA/SCL) and power.");
}

// Test 2: Read motion data and verify it appears valid.
void test_sensor_read_valid_data(void) {
    MotionData data;
    
    // Ensure the sensor read function succeeds.
    bool readSuccess = sensor.read(data);
    TEST_ASSERT_TRUE_MESSAGE(readSuccess, "Failed to read data from the sensor.");

    // Compute the vector magnitude of acceleration.
    float magnitude = sqrt(data.accelX * data.accelX + 
                           data.accelY * data.accelY + 
                           data.accelZ * data.accelZ);
    
    // Ensure the data includes a gravity-level acceleration reading.
    bool hasGravity = (magnitude > 0.5f);
    TEST_ASSERT_TRUE_MESSAGE(hasGravity, "Sensor data is 0! Is it sleeping/broken?");
}

void setup() {
    delay(2000); // Allow the board and sensor to wake after power-up.
    UNITY_BEGIN();
    
    RUN_TEST(test_sensor_connection);
    RUN_TEST(test_sensor_read_valid_data);
    
    UNITY_END();
    delay(1000); // Critical: Wait for Serial buffer to flush before board resets.
}

void loop() {
    // Empty loop for Unity tests.
}