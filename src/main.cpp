// ========================================================================================================
// File: main.cpp
// Purpose: Production composition root. Wires hardware sensors, ISR timers, ring buffers, streaming, 
// and the DSP classification engine into a non-blocking 100Hz pipeline.
// ========================================================================================================

#include <Arduino.h>

// Use the correct PlatformIO unit testing flag to prevent multiple definitions
#ifndef PIO_UNIT_TESTING

#include "params/AlgoParams.hpp"
#include "config/HardwareConfig.hpp"
#include "params/BuildFlags.h"

#include "hal/SampleTimer.h"
#include "buffers/RingBuffer.hpp"
#include "streaming/SerialStreamer.h"
#include "streaming/DebugCli.h"

#include "algorithem/MotionClassifier.h"
#include "filter/EmaFilter.hpp"
#include "detectors/StepMotion.hpp"
#include "detectors/JumpMotion.hpp"
#include "detectors/StairMotion.hpp"

#if USE_MOCK_SENSOR
#include "sensors/MockSensor.h"
MotionSensor* sensor = getSensor();
#else
#include "drivers/Mpu6050Sensor.h"
Mpu6050Sensor hardwareSensor;
MotionSensor* sensor = &hardwareSensor;
#endif

SampleTimer sampleTimer;
SampleRingBuffer<AlgoParams::RING_BUFFER_CAPACITY> ringBuffer;
SerialStreamer streamer;
DebugCli cli;

EmaFilter mainFilter(0.53f);
MotionClassifier classifier(&mainFilter);
StepMotion stepDetector;
JumpMotion jumpDetector;
StairMotion stairDetector;

uint32_t totalSamplesProcessed = 0;
uint32_t ledTurnOffTimestampMs = 0;
bool isLedActive = false;

void setup() {
    Serial.begin(AlgoParams::SERIAL_BAUD);
    delay(2000);

    Serial.println("=================================================");
    Serial.println("StepMaster Ankle Motion Tracker - Initializing...");
    Serial.println("=================================================");

    pinMode(HardwareConfig::LED_PIN, OUTPUT);
    digitalWrite(HardwareConfig::LED_PIN, HardwareConfig::LED_OFF);

    streamer.begin();
    cli.begin(&streamer);

    classifier.addDetector(&jumpDetector);
    classifier.addDetector(&stairDetector);
    classifier.addDetector(&stepDetector);

    if (!sensor->begin()) {
        Serial.println("[ERROR] Sensor initialization failed! Check I2C wiring.");
        while (true) { delay(100); }
    }
    
    Serial.println("[INFO] Sensor initialized successfully.");

    if (!sampleTimer.begin(AlgoParams::SAMPLE_INTERVAL_US)) {
        Serial.println("[ERROR] Failed to start hardware timer.");
        while (true) { delay(100); }
    }

    Serial.println("[INFO] Timer running at 100Hz. Entering main monitoring loop...");
}

void loop() {
    cli.poll();

    // Consume the timer tick securely to prevent ISR race conditions
    if (sampleTimer.consumeSample()) {
        MotionData rawData;
        if (sensor->read(rawData)) {
            RawSample rSample;
            rSample.sequence = rawData.sequence;
            rSample.timestampMs = rawData.timestampMs;
            rSample.ax = rawData.accelX;
            rSample.ay = rawData.accelY;
            rSample.az = rawData.accelZ;
            rSample.gx = rawData.gyroX;
            rSample.gy = rawData.gyroY;
            rSample.gz = rawData.gyroZ;

            ringBuffer.push(rSample);
        }
    }

    while (!ringBuffer.empty()) {
        RawSample currentSample;
        ringBuffer.pop(currentSample);

        streamer.streamSample(currentSample);

        MotionData mData;
        mData.timestampMs = currentSample.timestampMs;
        mData.sequence = currentSample.sequence;
        mData.accelX = currentSample.ax;
        mData.accelY = currentSample.ay;
        mData.accelZ = currentSample.az;
        mData.gyroX = currentSample.gx;
        mData.gyroY = currentSample.gy;
        mData.gyroZ = currentSample.gz;

        MotionState currentState = classifier.update(mData);

        if (currentState != MotionState::IDLE && currentState != MotionState::NOISE) {
            digitalWrite(HardwareConfig::LED_PIN, HardwareConfig::LED_ON);
            ledTurnOffTimestampMs = millis() + AlgoParams::LED_PULSE_DURATION_MS;
            isLedActive = true;

            Serial.print("[EVENT] State: ");
            Serial.print(static_cast<int>(currentState));
            Serial.print(" | Steps: "); Serial.print(stepDetector.getCount());
            Serial.print(" | Jumps: "); Serial.print(jumpDetector.getCount());
            Serial.print(" | Stairs: "); Serial.println(stairDetector.getCount());
        }

        totalSamplesProcessed++;
    }

    if (isLedActive && millis() >= ledTurnOffTimestampMs) {
        digitalWrite(HardwareConfig::LED_PIN, HardwareConfig::LED_OFF);
        isLedActive = false;
    }

    uint32_t currentI2cErrors = sensor->i2cErrorCount();
    cli.setSampleCount(totalSamplesProcessed);
    cli.setI2cErrors(currentI2cErrors);
    streamer.maybePrintStats(totalSamplesProcessed, currentI2cErrors);
}

#endif // PIO_UNIT_TESTING