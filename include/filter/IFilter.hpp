// ========================================================================================================
// File: IFilter.hpp
// Purpose: Abstract interface for signal filtering to adhere to the Open/Closed Principle.
// ========================================================================================================

#ifndef I_FILTER_HPP
#define I_FILTER_HPP

// ----------------------------------------------------------------------------------------------------
// Interface: IFilter
// Purpose: Defines a standard contract for any signal processing filter (e.g., EMA, High-Pass).
// ----------------------------------------------------------------------------------------------------
class IFilter {
public:
    // Virtual destructor to ensure proper cleanup of derived classes.
    virtual ~IFilter() = default;
    
    // Processes a single raw input sample and returns the filtered output.
    virtual float process(float input) = 0;
};

#endif // I_FILTER_HPP