// ========================================================================================================
// File: RuleBasedFSMClassifier.hpp
// Purpose: Concrete implementation of IClassifier utilizing polymorphic FSM detectors (Step, Jump, Stair).
// ========================================================================================================

#ifndef RULE_BASED_FSM_CLASSIFIER_HPP
#define RULE_BASED_FSM_CLASSIFIER_HPP

#include <cstdint>
#include "algorithm/IClassifier.hpp"
#include "detectors/BaseMotion.hpp"

class RuleBasedFSMClassifier : public IClassifier {
public:
    RuleBasedFSMClassifier();
    
    // Registers a polymorphic motion detector into the priority array
    void addDetector(BaseMotion* detector);

    // Fulfills the IClassifier contract
    MotionState evaluate(const MotionFeatures& features, const AlgoConfig& config) override;

private:
    static const uint8_t MAX_DETECTORS = 5;
    BaseMotion* detectors_[MAX_DETECTORS];
    uint8_t detectorCount_;
};

#endif // RULE_BASED_FSM_CLASSIFIER_HPP