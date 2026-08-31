// ========================================================================================================
// File: MotionState.h
// Purpose: Defines the shared enumeration for user motion states across the StepMaster system.
// ========================================================================================================

#ifndef MOTION_STATE_H
#define MOTION_STATE_H

// Define an enumeration for the possible motion states identified by the system.
enum class MotionState {
    // Represents a state where no significant activity is detected.
    IDLE,
    
    // Represents a valid walking or running step.
    STEP,
    
    // Represents an airborne jump event.
    JUMP,
    
    // Represents invalid movements like kicks, fidgeting, or vehicle vibrations.
    NOISE
};

#endif // MOTION_STATE_H