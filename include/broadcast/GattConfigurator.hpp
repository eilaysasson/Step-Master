// ========================================================================================================
// File: GattConfigurator.hpp
// Purpose: Configures BLE services and characteristics, including Nordic OTA DFU support.
// ========================================================================================================

#ifndef GATT_CONFIGURATOR_HPP
#define GATT_CONFIGURATOR_HPP

#include <ArduinoBLE.h>
#include "config/BLEConfig.hpp"
#include "params/MotionState.h"

// Include CMSIS Core for direct hardware register access
#include <nrf.h>

class GattConfigurator {
public:
    GattConfigurator() : 
        stepMasterService_(BLEConfig::SERVICE_UUID),
        stepsChar_(BLEConfig::CHAR_STEPS_UUID, BLERead | BLENotify, 15),
        jumpsChar_(BLEConfig::CHAR_JUMPS_UUID, BLERead | BLENotify, 15),
        stairsChar_(BLEConfig::CHAR_STAIRS_UUID, BLERead | BLENotify, 15),
        stateChar_(BLEConfig::CHAR_STATE_UUID, BLERead | BLENotify, 15),
        
        batteryService_(BLEConfig::BATTERY_SERVICE_UUID),
        batteryLevelChar_(BLEConfig::BATTERY_LEVEL_CHAR_UUID, BLERead | BLENotify),
        
        dfuService_(BLEConfig::DFU_SERVICE_UUID),
        dfuControlChar_(BLEConfig::DFU_CONTROL_CHAR_UUID, BLEWrite | BLEWriteWithoutResponse, 1),
        
        stepsNameDescriptor_("2901", "Step Count"),
        jumpsNameDescriptor_("2901", "Jump Count"),
        stairsNameDescriptor_("2901", "Stair Count"),
        stateNameDescriptor_("2901", "Motion State") {}

    void setupServices() {
        // --- 1. StepMaster Core Service ---
        BLE.setAdvertisedService(stepMasterService_);

        stepsChar_.addDescriptor(stepsNameDescriptor_);
        jumpsChar_.addDescriptor(jumpsNameDescriptor_);
        stairsChar_.addDescriptor(stairsNameDescriptor_);
        stateChar_.addDescriptor(stateNameDescriptor_);

        stepMasterService_.addCharacteristic(stepsChar_);
        stepMasterService_.addCharacteristic(jumpsChar_);
        stepMasterService_.addCharacteristic(stairsChar_);
        stepMasterService_.addCharacteristic(stateChar_);
        
        BLE.addService(stepMasterService_);

        // --- 2. Battery Service ---
        batteryService_.addCharacteristic(batteryLevelChar_);
        BLE.addService(batteryService_);

        // --- 3. OTA DFU Service ---
        // Direct CMSIS hardware register access to trigger Nordic DFU mode
        dfuControlChar_.setEventHandler(BLEWritten, [](BLEDevice central, BLECharacteristic characteristic) {
            NRF_POWER->GPREGRET = 0x57; 
            NVIC_SystemReset();
        });
        
        dfuService_.addCharacteristic(dfuControlChar_);
        BLE.addService(dfuService_);

        // --- Initialize default values ---
        stepsChar_.writeValue("0");
        jumpsChar_.writeValue("0");
        stairsChar_.writeValue("0");
        stateChar_.writeValue("IDLE");
        batteryLevelChar_.writeValue(batteryLevelChar_.value());
    }

    BLEStringCharacteristic& getStepsChar() { return stepsChar_; }
    BLEStringCharacteristic& getJumpsChar() { return jumpsChar_; }
    BLEStringCharacteristic& getStairsChar() { return stairsChar_; }
    BLEStringCharacteristic& getStateChar() { return stateChar_; }

    bool isStepsSubscribed() { return stepsChar_.subscribed(); }
    bool isJumpsSubscribed() { return jumpsChar_.subscribed(); }
    bool isStairsSubscribed() { return stairsChar_.subscribed(); }
    bool isStateSubscribed() { return stateChar_.subscribed(); }

private:
    BLEService stepMasterService_;
    BLEStringCharacteristic stepsChar_;
    BLEStringCharacteristic jumpsChar_;
    BLEStringCharacteristic stairsChar_;
    BLEStringCharacteristic stateChar_;
    
    BLEService batteryService_;
    BLEUnsignedCharCharacteristic batteryLevelChar_;

    // Nordic DFU
    BLEService dfuService_;
    BLECharacteristic dfuControlChar_;
    
    BLEDescriptor stepsNameDescriptor_;
    BLEDescriptor jumpsNameDescriptor_;
    BLEDescriptor stairsNameDescriptor_;
    BLEDescriptor stateNameDescriptor_;
};

#endif // GATT_CONFIGURATOR_HPP