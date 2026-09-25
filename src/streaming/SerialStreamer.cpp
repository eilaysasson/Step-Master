#include "streaming/SerialStreamer.h"

#include "params/AlgoParams.hpp"
#include "params/BuildFlags.h"
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
        return;
    }

    // Teleplot Format: >VariableName:Value
    // Stream accelerometer axes
    Serial.print(F(">Accel_X:"));
    Serial.println(sample.ax, STREAM_DECIMAL_PLACES);
    Serial.print(F(">Accel_Y:"));
    Serial.println(sample.ay, STREAM_DECIMAL_PLACES);
    Serial.print(F(">Accel_Z:"));
    Serial.println(sample.az, STREAM_DECIMAL_PLACES);

    // Stream gyroscope axes
    Serial.print(F(">Gyro_X:")); Serial.println(sample.gx, STREAM_DECIMAL_PLACES);
    Serial.print(F(">Gyro_Y:")); Serial.println(sample.gy, STREAM_DECIMAL_PLACES);
    Serial.print(F(">Gyro_Z:")); Serial.println(sample.gz, STREAM_DECIMAL_PLACES);
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