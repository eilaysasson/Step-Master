// ========================================================================================================
// File: SystemManager.hpp
// Purpose: Central Application Controller. Acts as the mediator between hardware, algorithms, and BLE.
// ========================================================================================================

#ifndef SYSTEM_MANAGER_HPP
#define SYSTEM_MANAGER_HPP

#include <cstdint>

#include "params/AlgoParams.hpp"
#include "config/HardwareConfig.hpp"
#include "params/BuildFlags.h"
#include "config/AlgoConfig.hpp"

#include "utils/SampleTimer.h"
#include "buffers/RingBuffer.hpp"
#include "streaming/SerialStreamer.h"
#include "streaming/DebugCli.h"
#include "streaming/BLEManager.h" 

#include "algorithm/FeatureExtractor.hpp"
#include "algorithm/IClassifier.hpp"
#include "algorithm/RuleBasedFSMClassifier.hpp"
#include "filter/EmaFilter.hpp"
#include "detectors/StepMotion.hpp"
#include "detectors/JumpMotion.hpp"
#include "detectors/StairMotion.hpp"
#include "sensors/MotionSensor.h"

class SystemManager {
public:
    SystemManager();
    void begin();
    void run();

private:
    void fetchSensorData();
    void processBuffer();
    void handleVisualFeedback(MotionState currentState);

    AlgoConfig config_;

    SampleTimer sampleTimer_;
    SampleRingBuffer<AlgoParams::RING_BUFFER_CAPACITY> ringBuffer_;
    SerialStreamer streamer_;
    DebugCli cli_;
    
    // BLE Dependency Chain
    BleConnection bleConn_;
    GattConfigurator bleGatt_;
    DataBroadcaster bleBroadcaster_;
    BLEManager bleFacade_; 

    // Algorithm Pipeline
    EmaFilter mainFilter_;
    FeatureExtractor featureExtractor_;
    
    // Interface Abstraction Implementation
    RuleBasedFSMClassifier fsmEngine_;
    IClassifier* classifier_;
    
    StepMotion stepDetector_;
    JumpMotion jumpDetector_;
    StairMotion stairDetector_;

    MotionSensor* sensor_;

    // Internal State Tracking
    uint32_t totalSamplesProcessed_;
    uint32_t ledTurnOffTimestampMs_;
    bool isLedActive_;
    uint32_t globalSequence_;
};

#endif // SYSTEM_MANAGER_HPP