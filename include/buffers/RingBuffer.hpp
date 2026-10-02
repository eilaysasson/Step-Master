#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

#include <cstddef>
#include <cstdint>

struct RawSample {
    uint32_t sequence;
    uint32_t timestampMs;
    float ax, ay, az;
    float gx, gy, gz;
};

template <std::size_t Capacity>
class SampleRingBuffer {
public:
    void clear() {
        head_ = 0;
        tail_ = 0;
        count_ = 0;
        overflowCount_ = 0;
    }

    // dropOldest parameter provides a clear back-pressure policy
    bool push(const RawSample& sample, bool dropOldest = false) {
        if (count_ >= Capacity) {
            overflowCount_++;
            if (!dropOldest) {
                return false; 
            }
            // Drop oldest policy
            tail_ = (tail_ + 1) % Capacity;
            count_--;
        }

        buffer_[head_] = sample;
        head_ = (head_ + 1) % Capacity;
        ++count_;
        return true;
    }

    bool pop(RawSample& sample) {
        if (count_ == 0) return false;
        sample = buffer_[tail_];
        tail_ = (tail_ + 1) % Capacity;
        --count_;
        return true;
    }

    bool empty() const { return count_ == 0; }
    std::size_t size() const { return count_; }
    std::size_t capacity() const { return Capacity; }
    uint32_t getOverflowCount() const { return overflowCount_; }

private:
    RawSample buffer_[Capacity]{};
    std::size_t head_ = 0;
    std::size_t tail_ = 0;
    std::size_t count_ = 0;
    uint32_t overflowCount_ = 0;
};

#endif // RING_BUFFER_HPP