#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::vector<int> transformValues(const std::vector<int>& values) {
	std::vector<int> result;
	for (int value : values) {
		// TODO: transform each value and push it into result.
		(void)value;
	}
	return result;
}

int main() {
	const std::vector<int> values {3, 8, 13, 21};
	for (int value : transformValues(values)) {
		std::cout << value << "\n";
	}
}
