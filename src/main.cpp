// ========================================================================================================
// File: main.cpp
// Purpose: The absolute entry point. Instantiates the System Manager and delegates all execution.
// ========================================================================================================

#include <Arduino.h>

#ifndef PIO_UNIT_TESTING

#include "service/SystemManager.hpp"

// The central controller that manages all hardware, logic, and communications
SystemManager appManager;

void setup() {
    // Tell the manager to bootstrap the system
    appManager.begin();
}

void loop() {
    // Tell the manager to execute one cycle of the pipeline
    appManager.run();
}

#endif // PIO_UNIT_TESTING