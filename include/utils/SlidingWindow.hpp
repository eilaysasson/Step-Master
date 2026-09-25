#ifndef SLIDING_WINDOW_HPP
#define SLIDING_WINDOW_HPP

#include "params/AlgoParams.hpp"
#include <cstddef>

class SlidingWindow {
public:
    SlidingWindow();
    void push(float value);
    float getMean() const;
    float getStdDev() const;
    bool isFull() const;
    void clear();

private:
    float buffer_[AlgoParams::SLIDING_WINDOW_SIZE]{};
    std::size_t head_ = 0;
    std::size_t count_ = 0;
    float sum_ = 0.0f;
    float sumSq_ = 0.0f;
};

#endif // SLIDING_WINDOW_HPP