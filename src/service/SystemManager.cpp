// ========================================================================================================
// File: SystemManager.cpp
// Purpose: Implementation of the Application Controller. Instantiates all dependencies, wires them
// together, and runs the continuous hardware pipeline.
// ========================================================================================================

#include "service/SystemManager.hpp"
#include <Arduino.h>

#if USE_MOCK_SENSOR
#include "sensors/MockSensor.h"
#else
#include "drivers/Mpu6050Sensor.h"
#endif

// ----------------------------------------------------------------------------------------------------
// Constructor: Wires the dependencies together using constructor injection
// ----------------------------------------------------------------------------------------------------
SystemManager::SystemManager() 
    : bleBroadcaster_(bleGatt_, bleConn_),
      bleFacade_(bleConn_, bleGatt_, bleBroadcaster_),
      mainFilter_(AlgoParams::EMA_ALPHA), // Removed Magic Number: Now using AlgoParams::EMA_ALPHA
      featureExtractor_(&mainFilter_),
      fsmEngine_(),
      classifier_(&fsmEngine_), // Bind the abstract interface to the concrete implementation
      totalSamplesProcessed_(0),
      ledTurnOffTimestampMs_(0),
      isLedActive_(false),
      globalSequence_(0) 
{
#if USE_MOCK_SENSOR
    sensor_ = getSensor();
#else
    static Mpu6050Sensor hardwareSensor;
    sensor_ = &hardwareSensor;
#endif
}

void SystemManager::begin() {
    Serial.begin(AlgoParams::SERIAL_BAUD);
    delay(2000); 

    Serial.println("=================================================");
    Serial.println("StepMaster Ankle Motion Tracker - Initializing...");
    Serial.println("=================================================");

    pinMode(HardwareConfig::LED_PIN, OUTPUT);
    digitalWrite(HardwareConfig::LED_PIN, HardwareConfig::LED_OFF);

    streamer_.begin();
    cli_.begin(&streamer_);
    bleFacade_.begin(); 

    // Register detectors into the concrete FSM engine
    fsmEngine_.addDetector(&jumpDetector_);
    fsmEngine_.addDetector(&stairDetector_);
    fsmEngine_.addDetector(&stepDetector_);

    if (!sensor_->begin()) {
        Serial.println("[ERROR] Sensor initialization failed! Check I2C wiring.");
        while (true) { delay(100); }
    }
    Serial.println("[INFO] Sensor initialized successfully.");

    if (!sampleTimer_.begin(AlgoParams::SAMPLE_INTERVAL_US)) {
        Serial.println("[ERROR] Failed to start hardware timer.");
        while (true) { delay(100); }
    }

    Serial.println("[INFO] System Manager Ready. Running Main Loop...");
}

void SystemManager::run() {
    cli_.poll();
    bleFacade_.poll(); 

    fetchSensorData();
    processBuffer();

    if (isLedActive_ && millis() >= ledTurnOffTimestampMs_) {
        digitalWrite(HardwareConfig::LED_PIN, HardwareConfig::LED_OFF);
        isLedActive_ = false;
    }

    uint32_t currentI2cErrors = sensor_->i2cErrorCount();
    cli_.setSampleCount(totalSamplesProcessed_);
    cli_.setI2cErrors(currentI2cErrors);
    streamer_.maybePrintStats(totalSamplesProcessed_, currentI2cErrors);
}

void SystemManager::fetchSensorData() {
    if (sampleTimer_.consumeSample()) {
        MotionData rawData;
        if (sensor_->read(rawData)) {
            RawSample rSample;
            rSample.sequence = globalSequence_++;
            rSample.timestampMs = rawData.timestampMs; 
            rSample.ax = rawData.accelX;
            rSample.ay = rawData.accelY;
            rSample.az = rawData.accelZ;
            rSample.gx = rawData.gyroX;
            rSample.gy = rawData.gyroY;
            rSample.gz = rawData.gyroZ;

            ringBuffer_.push(rSample);
        }
    }
}

void SystemManager::processBuffer() {
    while (!ringBuffer_.empty()) {
        RawSample currentSample;
        ringBuffer_.pop(currentSample);

        streamer_.streamSample(currentSample);

        MotionData mData;
        mData.timestampMs = currentSample.timestampMs;
        mData.sequence = currentSample.sequence;
        mData.accelX = currentSample.ax;
        mData.accelY = currentSample.ay;
        mData.accelZ = currentSample.az;
        mData.gyroX = currentSample.gx;
        mData.gyroY = currentSample.gy;
        mData.gyroZ = currentSample.gz;

        MotionFeatures features = featureExtractor_.process(mData);
        
        // Execute classification cleanly through the abstract IClassifier interface
        MotionState currentState = classifier_->evaluate(features, config_);

        if (currentState != MotionState::IDLE && currentState != MotionState::NOISE) {
            handleVisualFeedback(currentState);

            uint32_t currentSteps = stepDetector_.getCount();
            uint32_t currentJumps = jumpDetector_.getCount();
            uint32_t currentStairs = stairDetector_.getCount();

            bleFacade_.updateTelemetry(currentState, currentSteps, currentJumps, currentStairs); 
        }

        totalSamplesProcessed_++;
    }
}

void SystemManager::handleVisualFeedback(MotionState currentState) {
    digitalWrite(HardwareConfig::LED_PIN, HardwareConfig::LED_ON);
    ledTurnOffTimestampMs_ = millis() + AlgoParams::LED_PULSE_DURATION_MS;
    isLedActive_ = true;

    Serial.print("[EVENT] State: "); Serial.print(static_cast<int>(currentState));
    Serial.print(" | Steps: "); Serial.print(stepDetector_.getCount());
    Serial.print(" | Jumps: "); Serial.print(jumpDetector_.getCount());
    Serial.print(" | Stairs: "); Serial.println(stairDetector_.getCount());
}