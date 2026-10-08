#include COURSE_HEADER
#include <algorithm>
#include <climits>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void verifyLinear(const std::vector<int>& input, int target) {
    const auto original = input;
    const auto found = std::find(input.begin(), input.end(), target);
    const std::size_t expected = static_cast<std::size_t>(found - input.begin());
    const auto report = findFirstLinear(input, target);
    require(report.index == expected, "linear first-match/absence contract");
    require(report.comparisons == (found == input.end() ? input.size() : expected + 1),
            "linear target-comparison contract");
    require(input == original, "linear changed input");
}

void verifyBoth(const std::vector<int>& input, int target) {
    verifyLinear(input, target);
    const auto original = input;
    const auto found = std::find(input.begin(), input.end(), target);
    const auto report = findFirstBinary(input, target);
    require(report.index == static_cast<std::size_t>(found - input.begin()),
            "binary first-match/absence contract");
    std::size_t upper = input.empty() ? 0 : 1;
    for (std::size_t width = input.size(); width > 0; width /= 2) ++upper;
    require(report.comparisons <= upper, "binary target-comparison upper bound");
    require(input.empty() ? report.comparisons == 0 : report.comparisons > 0,
            "binary zero/nonzero comparison boundary");
    require(input == original, "binary changed input");
}

int main() {
    for (const auto& values : std::vector<std::vector<int>>{
            {}, {4}, {4, 4, 4}, {INT_MIN, -1, 0, 0, INT_MAX}, {3, 8, 13, 21}}) {
        for (int target : {INT_MIN, -9, -1, 0, 3, 4, 8, 13, 21, 22, INT_MAX})
            verifyBoth(values, target);
    }
    const auto defaultLinear = findFirstLinear({3, 8, 13, 21}, 21);
    const auto defaultBinary = findFirstBinary({3, 8, 13, 21}, 21);
    require(defaultLinear.index == 3 && defaultLinear.comparisons == 4, "default linear trace");
    require(defaultBinary.index == 3 && defaultBinary.comparisons == 3, "default binary trace");
    std::mt19937 generator(20261008u);
    std::size_t pairs = 0;
    for (std::size_t size = 0; size <= 256; ++size) {
        std::vector<int> values;
        for (std::size_t index = 0; index < size; ++index)
            values.push_back(static_cast<int>(generator() % 17u) - 8);
        for (int target = -10; target <= 10; ++target) verifyLinear(values, target);
        std::sort(values.begin(), values.end());
        for (int target = -10; target <= 10; ++target) {
            verifyBoth(values, target);
            ++pairs;
        }
    }
    verifyBoth(std::vector<int>(1024, 4), 4);
    verifyBoth(std::vector<int>(1024, 4), 5);
    std::cout << "Search oracle passed: " << pairs
              << " generated vector/target pairs, duplicates, extremes, empty and unchanged input\n";
}
