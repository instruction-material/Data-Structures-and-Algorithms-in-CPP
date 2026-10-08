#ifndef DSCPP0_SEARCH_HPP
#define DSCPP0_SEARCH_HPP
#include <cstddef>
#include <vector>

// index == values.size() means absent. Neither algorithm changes the vector.
// comparisons counts value-to-target tests only, not validation, loops or output.
struct SearchReport {
    std::size_t index;
    std::size_t comparisons;
};

inline SearchReport findFirstLinear(const std::vector<int>& values, int target) {
    // TODO LINEAR: return the first matching index and count target comparisons.
    (void)target;
    return {values.size(), 0};
}

// Precondition: values is sorted in nondecreasing order. The driver checks it.
inline SearchReport findFirstBinary(const std::vector<int>& values, int target) {
    // TODO BINARY: return the first matching index and count target comparisons.
    (void)target;
    return {values.size(), 0};
}

#endif
