// ========================================================================================================
// File: EmaFilter.hpp
// Purpose: Concrete implementation of an Exponential Moving Average (EMA) filter.
// ========================================================================================================

#ifndef EMA_FILTER_HPP
#define EMA_FILTER_HPP

#include "IFilter.hpp"

// ----------------------------------------------------------------------------------------------------
// Class: EmaFilter
// Purpose: Applies an EMA filter to smooth high-frequency jitter without introducing significant phase delay.
// ----------------------------------------------------------------------------------------------------
class EmaFilter : public IFilter {
public:
    // Constructor initializes the filter's smoothing factor (alpha) and resets the previous output.
    EmaFilter(float alpha) : alpha_(alpha), previousOutput_(0.0f) {}

    // Applies the EMA formula: y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
    float process(float input) override {
        previousOutput_ = (alpha_ * input) + ((1.0f - alpha_) * previousOutput_);
        return previousOutput_;
    }

private:
    float alpha_;
    float previousOutput_;
};

#endif // EMA_FILTER_HPP