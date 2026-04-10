#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class QuickSortToolkit {
public:
	void add(int value) {
		values.push_back(value);
	}

	int medianOfThree(int left, int right) {
		const int middle = (left + right) / 2;
		// TODO: place the actual median at the middle index.
		return middle;
	}

	int partition(int left, int right, int pivotIndex) {
		(void) left;
		(void) right;
		return pivotIndex;
	}

	void sortAll() {
		// TODO: replace this with quicksort based on medianOfThree and partition.
		std::sort(values.begin(), values.end());
	}

	std::string toString() const {
		std::ostringstream out;
		for (std::size_t i = 0; i < values.size(); ++i) {
			if (i > 0) {
				out << ", ";
			}
			out << values[i];
		}
		return out.str();
	}

private:
	std::vector<int> values;
};

int main() {
	QuickSortToolkit toolkit;
	for (int value : { 7, 2, 9, 4, 1, 8 }) {
		toolkit.add(value);
	}

	toolkit.sortAll();
	std::cout << "Starter sorted values: " << toolkit.toString() << "\n";
}

