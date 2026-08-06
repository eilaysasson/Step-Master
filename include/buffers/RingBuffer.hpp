#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <cstddef>
#include <cstdint>

/// Raw sensor sample structure streamed from the IMU.
struct RawSample
{
    uint32_t sequence;
    uint32_t timestampMs;
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
};

template <std::size_t Capacity>
class SampleRingBuffer
{
public:
    /// Reset the ring buffer to an empty state.
    void clear()
    {
        head_ = 0;
        tail_ = 0;
        count_ = 0;
    }

    /// Add a raw sample to the buffer if there is room.
    /// Returns false when the buffer is full.
    bool push(const RawSample& sample)
    {
        if (count_ >= Capacity)
        {
            return false;
        }

        buffer_[head_] = sample;
        head_ = (head_ + 1) % Capacity;
        ++count_;
        return true;
    }

    /// Remove the oldest raw sample from the buffer.
    /// Returns false when the buffer is empty.
    bool pop(RawSample& sample)
    {
        if (count_ == 0)
        {
            return false;
        }

        sample = buffer_[tail_];
        tail_ = (tail_ + 1) % Capacity;
        --count_;
        return true;
    }

    /// True when there are no stored samples.
    bool empty() const
    {
        return count_ == 0;
    }

    /// Number of samples currently stored.
    std::size_t size() const
    {
        return count_;
    }

    /// Maximum number of samples the buffer can hold.
    std::size_t capacity() const
    {
        return Capacity;
    }

private:
    RawSample buffer_[Capacity]{};
    std::size_t head_ = 0;
    std::size_t tail_ = 0;
    std::size_t count_ = 0;
};

#endif
