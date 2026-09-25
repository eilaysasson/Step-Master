#ifndef PEAK_DETECTOR_HPP
#define PEAK_DETECTOR_HPP

class PeakDetector {
public:
    PeakDetector() : previousVal_(0.0f), prePreviousVal_(0.0f) {}

    bool update(float currentVal, float& outPeakValue) {
        bool peakFound = (previousVal_ > prePreviousVal_) && (previousVal_ > currentVal);
        outPeakValue = previousVal_;
        
        prePreviousVal_ = previousVal_;
        previousVal_ = currentVal;
        
        return peakFound;
    }

private:
    float previousVal_;
    float prePreviousVal_;
};

#endif // PEAK_DETECTOR_HPP