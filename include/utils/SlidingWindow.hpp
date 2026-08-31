// ========================================================================================================
// File: SlidingWindow.hpp
// Purpose: Maintains a rolling window of recent sensor data to calculate real-time mean and standard
// deviation in O(1) time complexity, using running sum and running sum of squares.
// ========================================================================================================

#ifndef SLIDING_WINDOW_HPP
#define SLIDING_WINDOW_HPP

// Include size_t for standard memory array indexing.
#include <cstddef>

// Include the algorithm parameters to access the sliding window size constant.
#include "AlgoParams.hpp"

// ----------------------------------------------------------------------------------------------------
// Class: SlidingWindow
// Purpose: A memory-efficient circular buffer that calculates statistical variance on the fly.
// ----------------------------------------------------------------------------------------------------
class SlidingWindow {
public:
    // ------------------------------------------------------------------------------------------------
    // Method: SlidingWindow()
    // Purpose: Constructor. Initializes the buffer array, running sums, and array indices to zero.
    // ------------------------------------------------------------------------------------------------
    SlidingWindow();

    // ------------------------------------------------------------------------------------------------
    // Method: void push(float value)
    // Purpose: Adds a new value to the window, replacing the oldest value if the buffer is full.
    // It updates the mathematical running sum and running sum of squares continuously.
    // ------------------------------------------------------------------------------------------------
    void push(float value);

    // ------------------------------------------------------------------------------------------------
    // Method: float getMean() const
    // Purpose: Calculates and returns the current arithmetic mean of the sliding window.
    // ------------------------------------------------------------------------------------------------
    float getMean() const;

    // ------------------------------------------------------------------------------------------------
    // Method: float getStdDev() const
    // Purpose: Calculates and returns the current standard deviation of the sliding window.
    // Uses max(0, variance) to prevent floating-point errors before applying the square root.
    // ------------------------------------------------------------------------------------------------
    float getStdDev() const;

    // ------------------------------------------------------------------------------------------------
    // Method: bool isFull() const
    // Purpose: Returns true if the buffer has collected enough samples to fill the entire window.
    // ------------------------------------------------------------------------------------------------
    bool isFull() const;

    // ------------------------------------------------------------------------------------------------
    // Method: void clear()
    // Purpose: Resets all internal state, sums, counters, and indices back to exactly zero.
    // ------------------------------------------------------------------------------------------------
    void clear();

private:
    // Circular buffer array to store the historical floating-point values.
    float buffer_[AlgoParams::SLIDING_WINDOW_SIZE];
    
    // Current logical index where the next incoming value will be inserted.
    std::size_t head_;
    
    // Number of active elements currently stored in the sliding window buffer.
    std::size_t count_;
    
    // The mathematical running sum of all active elements currently inside the window.
    float sum_;
    
    // The mathematical running sum of the squares of all active elements inside the window.
    float sumSq_;
};

#endif // SLIDING_WINDOW_HPP