#ifndef SERIAL_STREAMER_H
#define SERIAL_STREAMER_H

#include "buffers/RingBuffer.hpp"

#include <cstdint>

/// Handles sending raw sample data and periodic statistics over serial.
class SerialStreamer
{
public:
    /// Enable streaming and set initial internal timestamp.
    void begin();

    /// Enable or disable serial output at runtime.
    void setEnabled(bool enabled);

    /// Query whether streaming is currently enabled.
    bool enabled() const;

    /// Send one raw sample as CSV over the serial interface.
    void streamSample(const RawSample& sample);

    /// Print periodic statistics if the stats interval has elapsed.
    void maybePrintStats(uint32_t& samplesSinceLastStat, uint32_t i2cErrors);

private:
    bool enabled_ = true;
    uint32_t lastStatMs_ = 0;
};

#endif
