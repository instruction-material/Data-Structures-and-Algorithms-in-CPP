#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::vector<int> transformValues(const std::vector<int>& values) {
	std::vector<int> result;
	result.reserve(values.size());
	for (int value : values) {
		result.push_back(value * 2 + 1);
	}
	return result;
}

int main() {
	const std::vector<int> values {3, 8, 13, 21};
	for (int value : transformValues(values)) {
		std::cout << value << "\n";
	}
}
