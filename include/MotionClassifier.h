// ========================================================================================================
// File: MotionClassifier.h
// Purpose: Advanced classification engine implementing dual-path DSP, sliding window adaptive thresholds,
// and distinct Finite State Machines (FSM) for Step and Jump detection.
// ========================================================================================================

#ifndef MOTION_CLASSIFIER_H
#define MOTION_CLASSIFIER_H

// Include standard integer types.
#include <cstdint>

// Include the standard motion data structures.
#include "MotionSensor.h"

// Include the shared motion state enumeration.
#include "MotionState.h"

// Include the O(1) sliding window for adaptive threshold calculations.
#include "utils/SlidingWindow.hpp"

// Define internal states strictly for the Jump Finite State Machine.
enum class JumpFsmState {
    // Default state representing the user being on the ground.
    NORMAL,
    
    // Airborne state triggered when raw acceleration drops below the flight threshold.
    FLIGHT
};

// Main classification engine class.
class MotionClassifier {
public:
    // ----------------------------------------------------------------------------------------------------
    // Method: MotionClassifier()
    // Purpose: Constructor. Initializes the sliding window, EMA history, and FSM tracking variables.
    // ----------------------------------------------------------------------------------------------------
    MotionClassifier();

    // ----------------------------------------------------------------------------------------------------
    // Method: MotionState update(const MotionData& rawData)
    // Purpose: Processes a new raw frame through the dual-path DSP and evaluates FSMs to classify motion.
    // ----------------------------------------------------------------------------------------------------
    MotionState update(const MotionData& rawData);

private:
    // ----------------------------------------------------------------------------------------------------
    // Method: MotionState processJumpFsm(...)
    // Purpose: Evaluates raw magnitude against the Jump FSM to detect airborne and landing events.
    // ----------------------------------------------------------------------------------------------------
    MotionState processJumpFsm(float rawMag, uint32_t currentTimestampMs);

    // ----------------------------------------------------------------------------------------------------
    // Method: MotionState processStepFsm(...)
    // Purpose: Evaluates smoothed magnitude against the Step FSM using adaptive peak detection.
    // ----------------------------------------------------------------------------------------------------
    MotionState processStepFsm(float smoothMag, uint32_t currentTimestampMs);

    // O(1) sliding window instance to calculate rolling mean and standard deviation.
    SlidingWindow slidingWindow_;

    // Stores the previous EMA value required for the recursive low-pass filter formula.
    float previousEma_;

    // Stores the previous smoothed magnitude to identify local peaks (A[n-1] < A[n] > A[n+1]).
    float previousSmoothMag_;

    // Timestamp defining the exact moment the step refractory period (debounce) will expire.
    uint32_t stepRefractoryEndMs_;

    // Tracks the current active state of the internal Jump FSM.
    JumpFsmState jumpState_;

    // Timestamp marking the exact moment the FLIGHT state began.
    uint32_t flightStartMs_;

    // Counter tracking how many consecutive samples have exceeded the flight exit threshold.
    uint32_t consecutiveFlightExitSamples_;
};

#endif // MOTION_CLASSIFIER_H