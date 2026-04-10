#include <chrono>
#include <iostream>
#include <random>
#include <set>
#include <unordered_set>
#include <vector>

template <typename Callback>
long long timeRun(Callback callback) {
	const auto start = std::chrono::steady_clock::now();
	callback();
	const auto end = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

int main() {
	std::mt19937 rng(42);
	std::uniform_int_distribution<int> distribution(1, 100000);
	std::vector<int> values(2000);
	for (int& value : values) {
		value = distribution(rng);
	}

	const auto vectorInsert = timeRun([&] {
		std::vector<int> data;
		for (int value : values) {
			data.push_back(value);
		}
	});

	const auto setInsert = timeRun([&] {
		std::set<int> data;
		for (int value : values) {
			data.insert(value);
		}
	});

	const auto unorderedInsert = timeRun([&] {
		std::unordered_set<int> data;
		for (int value : values) {
			data.insert(value);
		}
	});

	std::cout << "vector insert (us): " << vectorInsert << "\n";
	std::cout << "set insert (us): " << setInsert << "\n";
	std::cout << "unordered_set insert (us): " << unorderedInsert << "\n";
	std::cout << "TODO: add custom linked-list, BST, and AVL comparisons.\n";
}

