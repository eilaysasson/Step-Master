#include "streaming/DebugCli.h"

#include "BuildFlags.h"
#include "hal/SampleTimer.h"
#include "streaming/SerialStreamer.h"

#include <Arduino.h>
#include <cstring>

void DebugCli::begin(SerialStreamer* streamer)
{
    streamer_ = streamer;
    lineLength_ = 0;
    rateWindowStartMs_ = millis();
    rateWindowSamples_ = 0;
}

void DebugCli::setSampleCount(uint32_t count)
{
    sampleCount_ = count;
}

void DebugCli::setI2cErrors(uint32_t errors)
{
    i2cErrors_ = errors;
}

void DebugCli::poll()
{
#if !ENABLE_DEBUG_CLI
    return;
#endif

    // Consume all available serial characters and build command lines.
    while (Serial.available() > 0)
    {
        const char c = static_cast<char>(Serial.read());
        if (c == '\r')
        {
            continue;
        }

        if (c == '\n')
        {
            lineBuffer_[lineLength_] = '\0';
            if (lineLength_ > 0)
            {
                processLine(lineBuffer_);
            }
            lineLength_ = 0;
            continue;
        }

        if (lineLength_ + 1 < sizeof(lineBuffer_))
        {
            lineBuffer_[lineLength_++] = c;
        }
    }
}

void DebugCli::processLine(const char* line)
{
    if (line[0] == '#')
    {
        // Allow comments or commands prefixed with '#'.
        ++line;
    }

    if (strcmp(line, "help") == 0)
    {
        printHelp();
    }
    else if (strcmp(line, "stats") == 0)
    {
        printStats();
    }
    else if (strcmp(line, "reset") == 0)
    {
        // Reset runtime counters used for diagnostics.
        g_sampleDrops = 0;
        g_expectedTicks = 0;
        sampleCount_ = 0;
        rateWindowStartMs_ = millis();
        rateWindowSamples_ = 0;
        Serial.println(F("OK counters reset"));
    }
    else if (strcmp(line, "stream on") == 0)
    {
        if (streamer_ != nullptr)
        {
            streamer_->setEnabled(true);
        }
        Serial.println(F("OK stream on"));
    }
    else if (strcmp(line, "stream off") == 0)
    {
        if (streamer_ != nullptr)
        {
            streamer_->setEnabled(false);
        }
        Serial.println(F("OK stream off"));
    }
    else if (strcmp(line, "rate") == 0)
    {
        printRate();
    }
    else
    {
        Serial.println(F("ERR unknown command (try help)"));
    }
}

void DebugCli::printHelp() const
{
    Serial.println(F("Commands:"));
    Serial.println(F("  help        - list commands"));
    Serial.println(F("  stats       - print counters"));
    Serial.println(F("  reset       - reset drop/sample counters"));
    Serial.println(F("  stream on   - enable CSV output"));
    Serial.println(F("  stream off  - disable CSV output"));
    Serial.println(F("  rate        - print measured sample rate"));
}

void DebugCli::printStats() const
{
    Serial.print(F("STATS samples="));
    Serial.print(sampleCount_);
    Serial.print(F(" drops="));
    Serial.print(g_sampleDrops);
    Serial.print(F(" expected_ticks="));
    Serial.print(g_expectedTicks);
    Serial.print(F(" i2c_errors="));
    Serial.println(i2cErrors_);
}

void DebugCli::printRate() const
{
    const uint32_t nowMs = millis();
    const uint32_t elapsedMs = nowMs - rateWindowStartMs_;
    if (elapsedMs == 0)
    {
        Serial.println(F("RATE unavailable"));
        return;
    }

    const float rateHz = (static_cast<float>(rateWindowSamples_) * 1000.0f)
        / static_cast<float>(elapsedMs);
    Serial.print(F("RATE hz="));
    Serial.println(rateHz, 2);
}
