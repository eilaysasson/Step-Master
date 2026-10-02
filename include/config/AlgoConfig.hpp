// ========================================================================================================
// File: AlgoConfig.hpp
// Purpose: Runtime parameter structure allowing dynamic field calibration without recompiling.
// ========================================================================================================

#ifndef ALGO_CONFIG_HPP
#define ALGO_CONFIG_HPP

#include <cstdint>

struct AlgoConfig {
    float stepThresholdMinG = 1.15f;
    float stepThresholdMaxG = 1.90f;
    float stepKFactor = 1.5f;
    
    float pitchThresholdStairs = 25.0f;
    float pitchThresholdJump = 65.0f;
    
    uint32_t stepRefractoryMs = 250;
};

#endif // ALGO_CONFIG_HPP