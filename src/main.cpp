#include <Arduino.h>

#include "BuildFlags.h"
#include "AlgoParams.hpp"
#include "MotionAlgorithms.h"
#include "MotionSensor.h"
#include "buffers/RingBuffer.hpp"
#include "config/HardwareConfig.hpp"
#if USE_MOCK_SENSOR
#include "MockSensor.h"
#else
#include "drivers/Mpu6050Sensor.h"
#endif
#include "hal/SampleTimer.h"
#include "streaming/DebugCli.h"
#include "streaming/SerialStreamer.h"

namespace
{
/// Ring buffer used to queue raw samples before streaming.
SampleRingBuffer<AlgoParams::RING_BUFFER_CAPACITY> sampleBuffer;

/// IMU sensor interface implementation.
#if USE_MOCK_SENSOR
MotionSensor* sensor = getSensor();
#else
Mpu6050Sensor realSensor;
MotionSensor* sensor = &realSensor;
#endif

/// Hardware timer that triggers sampling at a fixed interval.
SampleTimer sampleTimer;

/// Serial streamer used to output CSV samples and stats.
SerialStreamer streamer;

/// Debug CLI used for command processing over serial.
DebugCli cli;

/// Monotonic sequence number for each sample.
uint32_t sampleSequence = 0;

/// Count of samples queued since the last statistics report.
uint32_t statWindowSamples = 0;
/// Non-blocking LED state management for step detection
uint32_t ledTurnOffTimestampMs = 0;
bool isLedActive = false;
} // namespace
#ifndef UNIT_TEST
void setup()
{
    // Initialize LED Pin
    pinMode(HardwareConfig::LED_PIN, OUTPUT);
    digitalWrite(HardwareConfig::LED_PIN, HardwareConfig::LED_OFF);

    Serial.begin(AlgoParams::SERIAL_BAUD);
    delay(1000);

    Serial.println(F("Ankle Motion Tracker M1"));

    // Initialize the selected sensor implementation and fail fast if initialization fails.
    if (!sensor->begin())
    {
        Serial.println(F("ERR IMU init failed"));
        while (true)
        {
            delay(1000);
        }
    }

    sampleBuffer.clear();
    streamer.begin();
    cli.begin(&streamer);

    // Start the sample timer used to generate periodic sensor reads.
    if (!sampleTimer.begin(AlgoParams::SAMPLE_INTERVAL_US))
    {
        Serial.println(F("ERR sample timer init failed"));
        while (true)
        {
            delay(1000);
        }
    }

    Serial.println(F("READY streaming at 100 Hz"));
}

void loop()
{
    // Read sensor data only when the timer interrupt has signaled that a sample is due.
    if (g_sampleDue)
    {
        g_sampleDue = false;

        MotionData motion{};
        if (!sensor->read(motion))
        {
            // Preserve the current error count for CLI diagnostics.
            cli.setI2cErrors(sensor->i2cErrorCount());
        }
        else
        {
            // Evaluate movement/step detection using MotionAlgorithms engine
            if (detectMovement(motion))
            {
                digitalWrite(HardwareConfig::LED_PIN, HardwareConfig::LED_ON);
                ledTurnOffTimestampMs = millis() + AlgoParams::LED_PULSE_DURATION_MS;
                isLedActive = true;
            }
            // Populate a raw sample object from the latest motion data.
            RawSample sample{};
            sample.sequence = ++sampleSequence;
            sample.timestampMs = millis();
            sample.ax = motion.accelX;
            sample.ay = motion.accelY;
            sample.az = motion.accelZ;
            sample.gx = motion.gyroX;
            sample.gy = motion.gyroY;
            sample.gz = motion.gyroZ;

            // Attempt to queue the sample into the ring buffer.
            if (!sampleBuffer.push(sample))
            {
#if ENABLE_SAMPLE_DROP_COUNTER
                ++g_sampleDrops;
#endif
            }
            else
            {
                ++statWindowSamples;
            }
        }
    }
    // Non-blocking timer check to turn off the LED after duration elapses
    if (isLedActive && (millis() >= ledTurnOffTimestampMs))
    {
        digitalWrite(HardwareConfig::LED_PIN, HardwareConfig::LED_OFF);
        isLedActive = false;
    }

    // Send any queued samples over the serial stream.
    RawSample outgoing{};
    while (sampleBuffer.pop(outgoing))
    {
        streamer.streamSample(outgoing);
    }

    // Update CLI internal counters and process pending commands.
    cli.setSampleCount(sampleSequence);
    cli.setI2cErrors(sensor->i2cErrorCount());
    cli.poll();

    // Print periodic statistics if the stat interval has elapsed.
    streamer.maybePrintStats(statWindowSamples, sensor->i2cErrorCount());
}
#endif


