#ifndef BUILD_FLAGS_H
#define BUILD_FLAGS_H

#ifndef USE_MOCK_SENSOR
#define USE_MOCK_SENSOR 0
#endif

#define ENABLE_STREAMING 1
#define ENABLE_SAMPLE_DROP_COUNTER 1
#define ENABLE_DEBUG_CLI 1
#define STREAM_DECIMAL_PLACES 4

/// Compile-time sensor selection.
/// This is set by the PlatformIO environment build flags.
/// 0 = real IMU hardware, 1 = mock sensor mode.
#endif