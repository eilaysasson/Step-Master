#include "streaming/SerialStreamer.h"

#include "AlgoParams.hpp"
#include "BuildFlags.h"
#include "hal/SampleTimer.h"

#include <Arduino.h>

void SerialStreamer::begin()
{
    enabled_ = true;
    lastStatMs_ = millis();
}

void SerialStreamer::setEnabled(bool enabled)
{
    enabled_ = enabled;
}

bool SerialStreamer::enabled() const
{
    return enabled_;
}

void SerialStreamer::streamSample(const RawSample& sample)
{
#if ENABLE_STREAMING
    if (!enabled_)
    {
        // Skip serial output when streaming has been turned off.
        return;
    }

    // Print a CSV-formatted sample line for external logging.
    Serial.print(F("S,"));
    Serial.print(sample.sequence);
    Serial.print(F(","));
    Serial.print(sample.timestampMs);
    Serial.print(F(","));
    Serial.print(sample.ax, STREAM_DECIMAL_PLACES);
    Serial.print(F(","));
    Serial.print(sample.ay, STREAM_DECIMAL_PLACES);
    Serial.print(F(","));
    Serial.print(sample.az, STREAM_DECIMAL_PLACES);
    Serial.print(F(","));
    Serial.print(sample.gx, STREAM_DECIMAL_PLACES);
    Serial.print(F(","));
    Serial.print(sample.gy, STREAM_DECIMAL_PLACES);
    Serial.print(F(","));
    Serial.print(sample.gz, STREAM_DECIMAL_PLACES);
    Serial.println();
#endif
}

void SerialStreamer::maybePrintStats(uint32_t& samplesSinceLastStat, uint32_t i2cErrors)
{
    const uint32_t nowMs = millis();
    if ((nowMs - lastStatMs_) < AlgoParams::STAT_INTERVAL_MS)
    {
        return;
    }

    lastStatMs_ = nowMs;

    const float elapsedSec = AlgoParams::STAT_INTERVAL_MS / 1000.0f;
    const float rateHz = elapsedSec > 0.0f
        ? static_cast<float>(samplesSinceLastStat) / elapsedSec
        : 0.0f;

    Serial.print(F("STAT,samples="));
    Serial.print(samplesSinceLastStat);
    Serial.print(F(",drops="));
#if ENABLE_SAMPLE_DROP_COUNTER
    Serial.print(g_sampleDrops);
#else
    Serial.print(F("0"));
#endif
    Serial.print(F(",i2c_errors="));
    Serial.print(i2cErrors);
    Serial.print(F(",rate_hz="));
    Serial.println(rateHz, 1);

    // Reset the sample counter after emitting stats.
    samplesSinceLastStat = 0;
}
