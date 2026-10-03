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
    constexpr const char* SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0";
    constexpr const char* CHAR_STEPS_UUID = "12345678-1234-5678-1234-56789abcdef1";
    constexpr const char* CHAR_JUMPS_UUID = "12345678-1234-5678-1234-56789abcdef2";
    constexpr const char* CHAR_STAIRS_UUID = "12345678-1234-5678-1234-56789abcdef3";
    constexpr const char* CHAR_STATE_UUID = "12345678-1234-5678-1234-56789abcdef4";

    // ----------------------------------------------------------------------------------------------------
    // Standard SIG Battery Service UUIDs
    // ----------------------------------------------------------------------------------------------------
    constexpr const char* BATTERY_SERVICE_UUID = "180F";
    constexpr const char* BATTERY_LEVEL_CHAR_UUID = "2A19";

    // ----------------------------------------------------------------------------------------------------
    // Nordic Secure DFU Service UUIDs
    // ----------------------------------------------------------------------------------------------------
    constexpr const char* DFU_SERVICE_UUID = "FE59";
    constexpr const char* DFU_CONTROL_CHAR_UUID = "8EC90001-F315-4F60-9FB8-838830DAEA50";
}

#endif // BLE_CONFIG_HPP