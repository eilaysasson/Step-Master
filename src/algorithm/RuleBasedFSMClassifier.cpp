// ========================================================================================================
// File: RuleBasedFSMClassifier.cpp
// Purpose: Implementation of the stateless, rule-based decision engine.
// ========================================================================================================

#include "algorithm/RuleBasedFSMClassifier.hpp"

RuleBasedFSMClassifier::RuleBasedFSMClassifier() : detectorCount_(0) {
    for (uint8_t i = 0; i < MAX_DETECTORS; i++) {
        detectors_[i] = nullptr;
    }
}

void RuleBasedFSMClassifier::addDetector(BaseMotion* detector) {
    if (detectorCount_ < MAX_DETECTORS) {
        detectors_[detectorCount_++] = detector;
    }
}

MotionState RuleBasedFSMClassifier::evaluate(const MotionFeatures& features, const AlgoConfig& config) {
    for (uint8_t i = 0; i < detectorCount_; i++) {
        if (detectors_[i] != nullptr) {
            MotionState state = detectors_[i]->evaluate(features, config);
            if (state != MotionState::IDLE) {
                return state;
            }
        }
    }
    return MotionState::IDLE;
}