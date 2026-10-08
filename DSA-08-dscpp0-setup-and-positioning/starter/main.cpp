#include "search.hpp"
#include <algorithm>
#include <charconv>
#include <iostream>
#include <string_view>
#include <vector>

namespace {
constexpr std::size_t maxValues = 1024;

bool parseInt(std::string_view text, int& value) {
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    return error == std::errc{} && end == text.data() + text.size();
}

void printReport(std::string_view name, SearchReport result, std::size_t count) {
    std::cout << name << " index=";
    if (result.index == count) std::cout << "none";
    else std::cout << result.index;
    std::cout << " comparisons=" << result.comparisons << '\n';
}
}

int main(int argc, char* argv[]) {
    int target = 21;
    std::vector<int> values{3, 8, 13, 21};
    if (argc > 1) {
        if (static_cast<std::size_t>(argc - 2) > maxValues || !parseInt(argv[1], target)) {
            std::cerr << "error: expected an int target and at most 1024 sorted int values\n";
            return 2;
        }
        values.clear();
        for (int argument = 2; argument < argc; ++argument) {
            int value = 0;
            if (!parseInt(argv[argument], value)) {
                std::cerr << "error: each value must be a complete int token\n";
                return 2;
            }
            values.push_back(value);
        }
        if (!std::is_sorted(values.begin(), values.end())) {
            std::cerr << "error: values must be in nondecreasing order\n";
            return 2;
        }
    }
    printReport("linear", findFirstLinear(values, target), values.size());
    printReport("binary", findFirstBinary(values, target), values.size());
}
