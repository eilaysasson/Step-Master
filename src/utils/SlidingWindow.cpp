#include "utils/SlidingWindow.hpp"
#include <cmath>
#include <algorithm>

SlidingWindow::SlidingWindow() {
    clear();
}

void SlidingWindow::push(float value) {
    float oldVal = buffer_[head_];
    buffer_[head_] = value;
    
    head_++;
    if (head_ >= AlgoParams::SLIDING_WINDOW_SIZE) {
        head_ = 0;
    }
    
    if (count_ < AlgoParams::SLIDING_WINDOW_SIZE) {
        count_++;
    }
    
    sum_ = sum_ + value - oldVal;
    sumSq_ = sumSq_ + (value * value) - (oldVal * oldVal);
}

float SlidingWindow::getMean() const {
    if (count_ == 0) return 0.0f;
    return sum_ / static_cast<float>(count_);
}

float SlidingWindow::getStdDev() const {
    if (count_ == 0) return 0.0f;
    
    float mean = getMean();
    float variance = (sumSq_ / static_cast<float>(count_)) - (mean * mean);
    
    variance = std::max(0.0f, variance);
    return std::sqrt(variance);
}

bool SlidingWindow::isFull() const {
    return count_ == AlgoParams::SLIDING_WINDOW_SIZE;
}

void SlidingWindow::clear() {
    for (std::size_t i = 0; i < AlgoParams::SLIDING_WINDOW_SIZE; i++) {
        buffer_[i] = 0.0f;
    }
    head_ = 0;
    count_ = 0;
    sum_ = 0.0f;
    sumSq_ = 0.0f;
}