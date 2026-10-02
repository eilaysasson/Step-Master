// ========================================================================================================
// File: IClassifier.hpp
// Purpose: Pure abstract interface for motion classification. Ensures the Open/Closed Principle 
// by decoupling the classification engine from the surrounding hardware and BLE pipelines.
// ========================================================================================================

#ifndef I_CLASSIFIER_HPP
#define I_CLASSIFIER_HPP

#include "params/MotionState.h"
#include "sensors/MotionFeatures.h"
#include "config/AlgoConfig.hpp"

class IClassifier {
public:
    virtual ~IClassifier() = default;

    // ----------------------------------------------------------------------------------------------------
    // Method: evaluate
    // Purpose: Takes fully extracted features and dynamic configuration, returning a classified state.
    // ----------------------------------------------------------------------------------------------------
    virtual MotionState evaluate(const MotionFeatures& features, const AlgoConfig& config) = 0;
};

#endif // I_CLASSIFIER_HPP