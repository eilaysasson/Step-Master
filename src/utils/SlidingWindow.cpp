// ========================================================================================================
// File: SlidingWindow.cpp
// Purpose: Implementation of the O(1) mathematical rolling window logic for the StepMaster system.
// ========================================================================================================

// Include the corresponding header definition file.
#include "utils/SlidingWindow.hpp"

// Include the standard C math library for the square root function.
#include <cmath>

// ----------------------------------------------------------------------------------------------------
// Method: SlidingWindow::SlidingWindow()
// Purpose: Constructor. Sets the buffer elements and all tracking variables to their default zero state.
// ----------------------------------------------------------------------------------------------------
SlidingWindow::SlidingWindow() {
    // Call the clear method to ensure a clean starting state upon object instantiation.
    clear();
}

// ----------------------------------------------------------------------------------------------------
// Method: void SlidingWindow::push(float value)
// Purpose: Inserts a new float into the circular buffer. Subtracts the exiting value from the running 
// sums and adds the new value. Manages the circular index wrapping safely.
// ----------------------------------------------------------------------------------------------------
void SlidingWindow::push(float value) {
    // Retrieve the oldest value from the buffer that is about to be physically overwritten.
    float oldVal = buffer_[head_];
    
    // Overwrite the oldest value in the array with the incoming new sensor value.
    buffer_[head_] = value;
    
    // Advance the head index, wrapping around to zero if it reaches the window capacity.
    head_ = (head_ + 1) % AlgoParams::SLIDING_WINDOW_SIZE;
    
    // Increment the count of stored elements, capping it at the maximum sliding window size.
    if (count_ < AlgoParams::SLIDING_WINDOW_SIZE) {
        // Increment count since the buffer is not yet completely full.
        count_++;
    }
    
    // Update the running sum by subtracting the outgoing old value and adding the new one.
    sum_ = sum_ + value - oldVal;
    
    // Update the running sum of squares by subtracting the old square and adding the new square.
    sumSq_ = sumSq_ + (value * value) - (oldVal * oldVal);
}

// ----------------------------------------------------------------------------------------------------
// Method: float SlidingWindow::getMean() const
// Purpose: Divides the running sum by the current number of elements to yield the average.
// Returns 0.0f if the buffer is entirely empty to prevent a division by zero exception.
// ----------------------------------------------------------------------------------------------------
float SlidingWindow::getMean() const {
    // Return exactly zero if there are no elements to prevent mathematical division errors.
    if (count_ == 0) {
        // Return fallback value.
        return 0.0f;
    }
    
    // Divide the accumulated sum by the number of valid samples to find the mean.
    return sum_ / static_cast<float>(count_);
}

// ----------------------------------------------------------------------------------------------------
// Method: float SlidingWindow::getStdDev() const
// Purpose: Computes the variance using (sumSq / N) - (mean * mean), clamping negative values to zero, 
// and returns the square root as the standard deviation.
// ----------------------------------------------------------------------------------------------------
float SlidingWindow::getStdDev() const {
    // Return exactly zero if there are no elements to prevent mathematical division errors.
    if (count_ == 0) {
        // Return fallback value.
        return 0.0f;
    }
    
    // Retrieve the current arithmetic mean of the window.
    float mean = getMean();
    
    // Calculate the raw variance using the mathematical identity: Var = E[X^2] - (E[X])^2.
    float variance = (sumSq_ / static_cast<float>(count_)) - (mean * mean);
    
    // Ensure variance does not drop below zero due to floating-point imprecision.
    if (variance < 0.0f) {
        // Clamp the variance strictly to zero.
        variance = 0.0f;
    }
    
    // Return the standard deviation by taking the square root of the sanitized variance.
    return std::sqrt(variance);
}

// ----------------------------------------------------------------------------------------------------
// Method: bool SlidingWindow::isFull() const
// Purpose: Evaluates if the buffer has reached its predefined capacity (usually 100 samples).
// ----------------------------------------------------------------------------------------------------
bool SlidingWindow::isFull() const {
    // Check if the current element count perfectly matches the maximum defined window size.
    return count_ == AlgoParams::SLIDING_WINDOW_SIZE;
}

// ----------------------------------------------------------------------------------------------------
// Method: void SlidingWindow::clear()
// Purpose: Purges the buffer data and resets all mathematical trackers and indices to zero.
// ----------------------------------------------------------------------------------------------------
void SlidingWindow::clear() {
    // Iterate through every physical slot in the internal buffer array.
    for (std::size_t i = 0; i < AlgoParams::SLIDING_WINDOW_SIZE; i++) {
        // Set the value at the current index to exactly zero.
        buffer_[i] = 0.0f;
    }
    
    // Reset the insertion index back to the beginning of the array.
    head_ = 0;
    
    // Reset the element counter to indicate an empty buffer state.
    count_ = 0;
    
    // Zero out the mathematical running sum tracker.
    sum_ = 0.0f;
    
    // Zero out the mathematical running sum of squares tracker.
    sumSq_ = 0.0f;
}