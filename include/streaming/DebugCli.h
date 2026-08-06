#ifndef DEBUG_CLI_H
#define DEBUG_CLI_H

#include <cstddef>
#include <cstdint>

class SerialStreamer;

/// Simple command-line interface for debugging and runtime control.
class DebugCli
{
public:
    /// Attach the CLI to a serial streamer for runtime commands.
    void begin(SerialStreamer* streamer);

    /// Poll the serial port for commands and process them.
    void poll();

    /// Update the sample count shown in statistics output.
    void setSampleCount(uint32_t count);

    /// Update the I2C error count shown in statistics output.
    void setI2cErrors(uint32_t errors);

private:
    void processLine(const char* line);
    void printHelp() const;
    void printStats() const;
    void printRate() const;

    SerialStreamer* streamer_ = nullptr;
    char lineBuffer_[64];
    std::size_t lineLength_ = 0;

    uint32_t sampleCount_ = 0;
    uint32_t i2cErrors_ = 0;
    uint32_t rateWindowStartMs_ = 0;
    uint32_t rateWindowSamples_ = 0;
};

#endif
