#include "quicksort-under-test.hpp"
#include <climits>
#include <random>
#include <stdexcept>

namespace {
void check(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

QuickSortToolkit makeToolkit(const std::vector<int>& values) {
    QuickSortToolkit toolkit;
    for (int value : values) toolkit.add(value);
    return toolkit;
}

std::vector<int> readValues(const QuickSortToolkit& toolkit) {
    std::string value;
    std::istringstream input(toolkit.toString());
    std::vector<int> result;
    while (std::getline(input, value, ',')) result.push_back(std::stoi(value));
    return result;
}

#if QUICKSORT_REFERENCE
int sortCases = 0;
void verifySort(const std::vector<int>& input) {
    auto toolkit = makeToolkit(input);
    auto copy = toolkit;
    auto expected = input;
    std::sort(expected.begin(), expected.end());
    copy.sortAll();
    check(readValues(copy) == expected, "sort differs from independent oracle");
    check(readValues(toolkit) == input, "sorting a copy changed its source");
    copy.sortAll();
    check(readValues(copy) == expected, "repeat sorting changed result");
    copy.add(INT_MIN);
    expected.push_back(INT_MIN);
    std::sort(expected.begin(), expected.end());
    copy.sortAll();
    check(readValues(copy) == expected, "adding after sort lost a value");
    ++sortCases;
}

void enumerate(std::vector<int>& values, int remaining) {
    if (remaining == 0) {
        verifySort(values);
        return;
    }
    for (int value : {-1, 0, 1}) {
        values.push_back(value);
        enumerate(values, remaining - 1);
        values.pop_back();
    }
}

void verifyRange(const std::vector<int>& input, int left, int right,
                 int pivotIndex) {
    auto toolkit = makeToolkit(input);
    const int pivotValue = input[pivotIndex];
    const int boundary = toolkit.partition(left, right, pivotIndex);
    const auto actual = readValues(toolkit);
    check(left <= boundary && boundary <= right, "pivot escaped range");
    check(actual[boundary] == pivotValue, "pivot value was not retained");
    for (int i = 0; i < static_cast<int>(input.size()); ++i) {
        if (i < left || i > right) check(actual[i] == input[i], "outside range changed");
        if (left <= i && i < boundary) check(actual[i] < pivotValue, "left partition is not smaller");
        if (boundary < i && i <= right) check(actual[i] >= pivotValue, "right partition is smaller");
    }
    auto actualMultiset = actual;
    auto expectedMultiset = input;
    std::sort(actualMultiset.begin(), actualMultiset.end());
    std::sort(expectedMultiset.begin(), expectedMultiset.end());
    check(actualMultiset == expectedMultiset, "partition lost or added values");

    toolkit = makeToolkit(input);
    const int middle = (left + right) / 2;
    check(toolkit.medianOfThree(left, right) == middle, "unexpected sampled index");
    const auto sampled = readValues(toolkit);
    for (int i = 0; i < static_cast<int>(input.size()); ++i) {
        if (i != left && i != middle && i != right)
            check(sampled[i] == input[i], "pivot selection changed unsampled entry");
    }
    if (right - left >= 2) {
        std::vector<int> expectedSamples{input[left], input[middle], input[right]};
        std::sort(expectedSamples.begin(), expectedSamples.end());
        check(sampled[left] == expectedSamples[0] && sampled[middle] == expectedSamples[1]
              && sampled[right] == expectedSamples[2], "samples do not match independently ordered values");
    } else {
        check(sampled[left] == std::min(input[left], input[right])
              && sampled[right] == std::max(input[left], input[right]), "tiny-range rule differs");
    }
}
#endif
} // namespace

int main() {
#if QUICKSORT_REFERENCE
    std::vector<int> values;
    for (int length = 0; length <= 7; ++length) enumerate(values, length);
    std::mt19937 random(20261009);
    for (int trial = 0; trial < 512; ++trial) {
        values.clear();
        const int count = 3 + static_cast<int>(random() % 94);
        for (int i = 0; i < count; ++i) values.push_back(static_cast<int>(random() % 31) - 15);
        verifySort(values);
        const int left = static_cast<int>(random() % count);
        const int right = left + static_cast<int>(random() % (count - left));
        const int pivot = left + static_cast<int>(random() % (right - left + 1));
        verifyRange(values, left, right, pivot);
    }
    for (int count : {0, 1, 2, 32, 96, 128}) {
        values.assign(count, 7);
        verifySort(values);
        for (int i = 0; i < count; ++i) values[i] = i - count / 2;
        verifySort(values);
        std::reverse(values.begin(), values.end());
        verifySort(values);
        for (int i = 0; i < count; ++i) values[i] = i % 2 == 0 ? INT_MIN : INT_MAX;
        verifySort(values);
    }
    verifyRange({7, 2, 9, 4, 1, 8}, 0, 5, 2);
    verifyRange({4, 1, 4, -2, 4}, 0, 4, 2);
    verifyRange({99, 4, 1, 4, -2, 88}, 1, 4, 3);
    verifyRange({INT_MAX}, 0, 0, 0);
    auto trace = makeToolkit({7, 2, 9, 4, 1, 8});
    check(trace.medianOfThree(0, 5) == 2, "worked pivot index differs");
    check(readValues(trace) == std::vector<int>({7, 2, 8, 4, 1, 9}), "worked median trace differs");
    check(trace.partition(0, 5, 2) == 4, "worked partition index differs");
    check(readValues(trace) == std::vector<int>({7, 2, 4, 1, 8, 9}), "worked partition trace differs");
    std::cout << "{\"role\":\"solution\",\"sortCases\":" << sortCases
              << ",\"partitionCases\":516,\"pivotCases\":516}\n";
#else
    for (const std::vector<int>& input : std::vector<std::vector<int>>{{}, {7}, {7, 2}, {7, 2, 9, 4, 1, 8}}) {
        auto toolkit = makeToolkit(input);
        toolkit.sortAll();
        check(readValues(toolkit) == input, "untouched learner task sorted its input");
        if (!input.empty()) {
            toolkit.medianOfThree(0, static_cast<int>(input.size()) - 1);
            toolkit.partition(0, static_cast<int>(input.size()) - 1, 0);
            check(readValues(toolkit) == input, "untouched learner task changed its input");
        }
    }
    std::cout << "{\"role\":\"starter\",\"unfinishedCases\":4}\n";
#endif
}
