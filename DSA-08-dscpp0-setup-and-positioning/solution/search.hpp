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
    std::size_t comparisons = 0;
    for (std::size_t index = 0; index < values.size(); ++index) {
        ++comparisons;
        if (values[index] == target) return {index, comparisons};
    }
    return {values.size(), comparisons};
}

// Precondition: values is sorted in nondecreasing order. The driver checks it.
inline SearchReport findFirstBinary(const std::vector<int>& values, int target) {
    std::size_t first = 0;
    std::size_t last = values.size();
    std::size_t comparisons = 0;
    while (first < last) {
        const std::size_t middle = first + (last - first) / 2;
        ++comparisons;
        if (values[middle] < target) first = middle + 1;
        else last = middle;
    }
    if (first < values.size()) {
        ++comparisons;
        if (values[first] == target) return {first, comparisons};
    }
    return {values.size(), comparisons};
}

#endif
