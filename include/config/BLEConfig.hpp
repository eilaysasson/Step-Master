// ========================================================================================================
// File: BLEConfig.hpp
// Purpose: Defines UUIDs and configuration parameters for the StepMaster BLE GATT Services.
// ========================================================================================================

#ifndef BLE_CONFIG_HPP
#define BLE_CONFIG_HPP

namespace BLEConfig {
    // Device Name as it will appear in Bluetooth scans
    constexpr const char* DEVICE_NAME = "StepMaster_XIAO";

    // ----------------------------------------------------------------------------------------------------
    // Custom StepMaster Service UUIDs
    // ----------------------------------------------------------------------------------------------------
    // Primary Service UUID for StepMaster telemetry
    constexpr const char* SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0";
    
    // Characteristic UUID for transmitting total steps
    constexpr const char* CHAR_STEPS_UUID = "12345678-1234-5678-1234-56789abcdef1";
    
    // Characteristic UUID for transmitting total jumps
    constexpr const char* CHAR_JUMPS_UUID = "12345678-1234-5678-1234-56789abcdef2";
    
    // Characteristic UUID for transmitting total stairs
    constexpr const char* CHAR_STAIRS_UUID = "12345678-1234-5678-1234-56789abcdef3";
    
    // Characteristic UUID for transmitting the current motion state
    constexpr const char* CHAR_STATE_UUID = "12345678-1234-5678-1234-56789abcdef4";

    // ----------------------------------------------------------------------------------------------------
    // Standard SIG Battery Service UUIDs
    // ----------------------------------------------------------------------------------------------------
    constexpr const char* BATTERY_SERVICE_UUID = "180F";
    constexpr const char* BATTERY_LEVEL_CHAR_UUID = "2A19";
}

#endif // BLE_CONFIG_HPP