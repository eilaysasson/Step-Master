#ifndef HARDWARE_CONFIG_HPP
#define HARDWARE_CONFIG_HPP

#include <Arduino.h>

namespace HardwareConfig
{
/// Built-in LED pin for Seeed Studio XIAO nRF52840
constexpr uint8_t LED_PIN = LED_BUILTIN;

/// Active-LOW logic configuration for onboard LED
constexpr uint8_t LED_ON = LOW;
constexpr uint8_t LED_OFF = HIGH;
} // namespace HardwareConfig

#endif // HARDWARE_CONFIG_HPP