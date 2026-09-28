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

        stepsChar_.addDescriptor(stepsNameDescriptor_);
        jumpsChar_.addDescriptor(jumpsNameDescriptor_);
        stairsChar_.addDescriptor(stairsNameDescriptor_);
        stateChar_.addDescriptor(stateNameDescriptor_);

        stepMasterService_.addCharacteristic(stepsChar_);
        stepMasterService_.addCharacteristic(jumpsChar_);
        stepMasterService_.addCharacteristic(stairsChar_);
        stepMasterService_.addCharacteristic(stateChar_);
        
        BLE.addService(stepMasterService_);

        batteryService_.addCharacteristic(batteryLevelChar_);
        BLE.addService(batteryService_);

        // Initialize with default readable text values
        stepsChar_.writeValue("0");
        jumpsChar_.writeValue("0");
        stairsChar_.writeValue("0");
        stateChar_.writeValue("IDLE");
        batteryLevelChar_.writeValue(100);
        
        BLE.advertise();
    }

    // Accessors modified to return BLEStringCharacteristic
    BLEStringCharacteristic& getStepsChar() { return stepsChar_; }
    BLEStringCharacteristic& getJumpsChar() { return jumpsChar_; }
    BLEStringCharacteristic& getStairsChar() { return stairsChar_; }
    BLEStringCharacteristic& getStateChar() { return stateChar_; }

    bool isStepsSubscribed() { return stepsChar_.subscribed(); }
    bool isJumpsSubscribed() { return jumpsChar_.subscribed(); }
    bool isStairsSubscribed() { return stairsChar_.subscribed(); }
    bool isStateSubscribed() { return stateChar_.subscribed(); }

private:
    GattConfigurator() : 
        stepMasterService_(BLEConfig::SERVICE_UUID),
        // Max 15 bytes string length - highly optimized
        stepsChar_(BLEConfig::CHAR_STEPS_UUID, BLERead | BLENotify, 15),
        jumpsChar_(BLEConfig::CHAR_JUMPS_UUID, BLERead | BLENotify, 15),
        stairsChar_(BLEConfig::CHAR_STAIRS_UUID, BLERead | BLENotify, 15),
        stateChar_(BLEConfig::CHAR_STATE_UUID, BLERead | BLENotify, 15),
        batteryService_(BLEConfig::BATTERY_SERVICE_UUID),
        batteryLevelChar_(BLEConfig::BATTERY_LEVEL_CHAR_UUID, BLERead | BLENotify),
        stepsNameDescriptor_("2901", "Step Count"),
        jumpsNameDescriptor_("2901", "Jump Count"),
        stairsNameDescriptor_("2901", "Stair Count"),
        stateNameDescriptor_("2901", "Motion State") {}

    BLEService stepMasterService_;
    BLEStringCharacteristic stepsChar_;
    BLEStringCharacteristic jumpsChar_;
    BLEStringCharacteristic stairsChar_;
    BLEStringCharacteristic stateChar_;
    
    BLEService batteryService_;
    BLEUnsignedCharCharacteristic batteryLevelChar_;
    
    BLEDescriptor stepsNameDescriptor_;
    BLEDescriptor jumpsNameDescriptor_;
    BLEDescriptor stairsNameDescriptor_;
    BLEDescriptor stateNameDescriptor_;
};

#endif // GATT_CONFIGURATOR_HPP