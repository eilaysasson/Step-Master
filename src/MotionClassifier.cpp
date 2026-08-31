// ========================================================================================================
// File: MotionClassifier.cpp
// Purpose: Implements the dual-path DSP classification engine, adaptive thresholds, and FSMs.
// ========================================================================================================

// Include the corresponding header file.
#include "MotionClassifier.h"

// Include the algorithm parameters for thresholds and timing constants.
#include "config/AlgoParams.hpp"

// Include standard math library for mathematical operations.
#include <cmath>

// Include standard algorithms library for clamp and max functions.
#include <algorithm>

// ----------------------------------------------------------------------------------------------------
// Method: MotionClassifier::MotionClassifier()
// Purpose: Constructor. Initializes FSM states, EMA history, and timestamps to their default values.
// ----------------------------------------------------------------------------------------------------
MotionClassifier::MotionClassifier() {
    // Initialize the previous EMA value to 1.0g (standard gravity at rest).
    previousEma_ = 1.0f;

    // Initialize the previous smoothed magnitude to 1.0g.
    previousSmoothMag_ = 1.0f;

    // Set the step refractory end timestamp to zero.
    stepRefractoryEndMs_ = 0;

    // Set the initial jump FSM state to NORMAL (grounded).
    jumpState_ = JumpFsmState::NORMAL;

    // Initialize the flight start timestamp to zero.
    flightStartMs_ = 0;

    // Reset the counter for consecutive flight exit samples.
    consecutiveFlightExitSamples_ = 0;
}

// ----------------------------------------------------------------------------------------------------
// Method: MotionState MotionClassifier::update(const MotionData& rawData)
// Purpose: Executes the dual-path DSP, updating the Jump and Step FSMs to return the current state.
// ----------------------------------------------------------------------------------------------------
MotionState MotionClassifier::update(const MotionData& rawData) {
    // Calculate the sum of squares for the X, Y, and Z acceleration axes.
    float sumOfSquares = (rawData.accelX * rawData.accelX) + 
                         (rawData.accelY * rawData.accelY) + 
                         (rawData.accelZ * rawData.accelZ);

    // Calculate the raw vector magnitude by taking the square root of the sum.
    float rawMag = std::sqrt(sumOfSquares);

    // Define the EMA alpha constant for a 12Hz cutoff at 100Hz sampling rate.
    const float EMA_ALPHA = 0.53f;

    // Calculate the new smoothed magnitude using the EMA formula.
    float smoothMag = (EMA_ALPHA * rawMag) + ((1.0f - EMA_ALPHA) * previousEma_);

    // Save the newly calculated EMA value for the next cycle.
    previousEma_ = smoothMag;

    // Process the raw magnitude through the Jump FSM to check for airborne events.
    MotionState jumpResult = processJumpFsm(rawMag, rawData.timestampMs);

    // Process the smoothed magnitude through the Step FSM to check for walking/running.
    MotionState stepResult = processStepFsm(smoothMag, rawData.timestampMs);

    // Update the previous smoothed magnitude for the next peak detection cycle.
    previousSmoothMag_ = smoothMag;

    // If a jump was detected, it takes highest priority over other motions.
    if (jumpResult == MotionState::JUMP) {
        // Return the JUMP state.
        return MotionState::JUMP;
    }

    // If a step was detected, it takes priority over IDLE/NOISE.
    if (stepResult == MotionState::STEP) {
        // Return the STEP state.
        return MotionState::STEP;
    }

    // Determine if the current state is IDLE or NOISE based on the raw magnitude.
    if (rawMag > 0.9f && rawMag < 1.1f) {
        // Return IDLE since the sensor is resting near 1.0g.
        return MotionState::IDLE;
    }

    // Return NOISE for all other non-classified active movements.
    return MotionState::NOISE;
}

// ----------------------------------------------------------------------------------------------------
// Method: MotionState MotionClassifier::processJumpFsm(float rawMag, uint32_t currentTimestampMs)
// Purpose: Evaluates raw acceleration to detect flight phases and landing impacts for jump detection.
// ----------------------------------------------------------------------------------------------------
MotionState MotionClassifier::processJumpFsm(float rawMag, uint32_t currentTimestampMs) {
    // Switch execution path based on the current state of the Jump FSM.
    switch (jumpState_) {
        // Handle the NORMAL state where the user is assumed to be on the ground.
        case JumpFsmState::NORMAL:
            // Check if the raw magnitude drops below the flight entry threshold.
            if (rawMag < AlgoParams::FLIGHT_ENTER_G) {
                // Transition the FSM state to FLIGHT.
                jumpState_ = JumpFsmState::FLIGHT;

                // Record the exact timestamp when the flight phase began.
                flightStartMs_ = currentTimestampMs;

                // Reset the exit sample counter for the new flight phase.
                consecutiveFlightExitSamples_ = 0;
            }
            // Break out of the switch statement.
            break;

        // Handle the FLIGHT state where the user is airborne.
        case JumpFsmState::FLIGHT:
            // Check if the raw magnitude exceeds the flight exit threshold.
            if (rawMag > AlgoParams::FLIGHT_EXIT_G) {
                // Increment the counter for consecutive exit samples.
                consecutiveFlightExitSamples_++;
            } else {
                // Reset the exit counter if the magnitude drops back down.
                consecutiveFlightExitSamples_ = 0;
            }

            // Define the required number of consecutive samples to confirm landing.
            const uint32_t REQUIRED_EXIT_SAMPLES = 3;

            // Check if enough consecutive samples have crossed the exit threshold.
            if (consecutiveFlightExitSamples_ >= REQUIRED_EXIT_SAMPLES) {
                // Calculate the total duration of the flight phase in milliseconds.
                uint32_t flightDuration = currentTimestampMs - flightStartMs_;

                // Transition the FSM state back to NORMAL.
                jumpState_ = JumpFsmState::NORMAL;

                // Check if the flight duration falls within the valid jump window.
                if (flightDuration >= AlgoParams::FLIGHT_MIN_DURATION_MS && 
                    flightDuration <= AlgoParams::FLIGHT_MAX_DURATION_MS) {
                    // Return JUMP as a valid jump event was completed.
                    return MotionState::JUMP;
                }
            }
            // Break out of the switch statement.
            break;
    }

    // Return IDLE by default if no jump event was completed in this cycle.
    return MotionState::IDLE;
}

// ----------------------------------------------------------------------------------------------------
// Method: MotionState MotionClassifier::processStepFsm(float smoothMag, uint32_t currentTimestampMs)
// Purpose: Uses the sliding window and peak detection to identify steps dynamically.
// ----------------------------------------------------------------------------------------------------
MotionState MotionClassifier::processStepFsm(float smoothMag, uint32_t currentTimestampMs) {
    // Push the new smoothed magnitude into the O(1) sliding window.
    slidingWindow_.push(smoothMag);

    // Retrieve the current rolling mean from the sliding window.
    float mean = slidingWindow_.getMean();

    // Retrieve the current rolling standard deviation from the sliding window.
    float stdDev = slidingWindow_.getStdDev();

    // Calculate the dynamic threshold using the mu + k * sigma formula.
    float dynamicThreshold = mean + (AlgoParams::STEP_K_FACTOR * stdDev);

    // Clamp the dynamic threshold between the predefined minimum and maximum limits.
    float clampedThreshold = std::max(AlgoParams::STEP_THRESHOLD_MIN_G
        ,std::min(dynamicThreshold, AlgoParams::STEP_THRESHOLD_MAX_G));

    // Static variable to preserve the pre-previous magnitude across function calls.
    static float olderMag = 1.0f;

    // Check if the previous sample is a local peak (higher than both neighbors).
    bool isPeak = (previousSmoothMag_ > olderMag) && (previousSmoothMag_ > smoothMag);

    // Update the older magnitude tracker for the next cycle.
    olderMag = previousSmoothMag_;

    // Check if a peak was found and its value exceeds the adaptive clamped threshold.
    if (isPeak && (previousSmoothMag_ > clampedThreshold)) {
        // Check if the refractory period (debounce timer) has expired.
        if (currentTimestampMs > stepRefractoryEndMs_) {
            // Set the new refractory end time to prevent immediate double-counting.
            stepRefractoryEndMs_ = currentTimestampMs + AlgoParams::STEP_REFRACTORY_MS;

            // Return STEP as a valid walking or running step was detected.
            return MotionState::STEP;
        }
    }

    // Return IDLE by default if no step was detected in this cycle.
    return MotionState::IDLE;
}