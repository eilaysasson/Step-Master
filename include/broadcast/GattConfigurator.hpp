// ========================================================================================================
// File: GattConfigurator.hpp
// Purpose: Singleton class responsible for defining and registering the BLE GATT services and characteristics.
// ========================================================================================================

#ifndef GATT_CONFIGURATOR_HPP
#define GATT_CONFIGURATOR_HPP

#include <ArduinoBLE.h>
#include "config/BLEConfig.hpp"
#include "params/MotionState.h"

class GattConfigurator {
public:
    static GattConfigurator& getInstance() {
        static GattConfigurator instance;
        return instance;
    }

    GattConfigurator(const GattConfigurator&) = delete;
    GattConfigurator& operator=(const GattConfigurator&) = delete;

    void setupServices() {
        BLE.setAdvertisedService(stepMasterService_);
        
        stepMasterService_.addCharacteristic(stepsChar_);
        stepMasterService_.addCharacteristic(jumpsChar_);
        stepMasterService_.addCharacteristic(stairsChar_);
        stepMasterService_.addCharacteristic(stateChar_);
        
        BLE.addService(stepMasterService_);

        batteryService_.addCharacteristic(batteryLevelChar_);
        BLE.addService(batteryService_);

        stepsChar_.writeValue(0);
        jumpsChar_.writeValue(0);
        stairsChar_.writeValue(0);
        stateChar_.writeValue(static_cast<uint8_t>(MotionState::IDLE));
        batteryLevelChar_.writeValue(100);
    }

    BLEUnsignedIntCharacteristic& getStepsChar() { return stepsChar_; }
    BLEUnsignedIntCharacteristic& getJumpsChar() { return jumpsChar_; }
    BLEUnsignedIntCharacteristic& getStairsChar() { return stairsChar_; }
    BLEByteCharacteristic& getStateChar() { return stateChar_; }

    // Check if the central device is actually subscribed to notifications
    bool isStepsSubscribed() { return stepsChar_.subscribed(); }
    bool isJumpsSubscribed() { return jumpsChar_.subscribed(); }
    bool isStairsSubscribed() { return stairsChar_.subscribed(); }
    bool isStateSubscribed() { return stateChar_.subscribed(); }

private:
    GattConfigurator() : 
        stepMasterService_(BLEConfig::SERVICE_UUID),
        stepsChar_(BLEConfig::CHAR_STEPS_UUID, BLERead | BLENotify),
        jumpsChar_(BLEConfig::CHAR_JUMPS_UUID, BLERead | BLENotify),
        stairsChar_(BLEConfig::CHAR_STAIRS_UUID, BLERead | BLENotify),
        stateChar_(BLEConfig::CHAR_STATE_UUID, BLERead | BLENotify),
        batteryService_(BLEConfig::BATTERY_SERVICE_UUID),
        batteryLevelChar_(BLEConfig::BATTERY_LEVEL_CHAR_UUID, BLERead | BLENotify) {}

    BLEService stepMasterService_;
    BLEUnsignedIntCharacteristic stepsChar_;
    BLEUnsignedIntCharacteristic jumpsChar_;
    BLEUnsignedIntCharacteristic stairsChar_;
    BLEByteCharacteristic stateChar_;
    
    BLEService batteryService_;
    BLEUnsignedCharCharacteristic batteryLevelChar_;
};

#endif // GATT_CONFIGURATOR_HPP