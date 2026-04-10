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
		if (values[middle] < values[left]) {
			std::swap(values[middle], values[left]);
		}
		if (values[right] < values[left]) {
			std::swap(values[right], values[left]);
		}
		if (values[right] < values[middle]) {
			std::swap(values[right], values[middle]);
		}
		return middle;
	}

	int partition(int left, int right, int pivotIndex) {
		const int pivotValue = values[pivotIndex];
		std::swap(values[pivotIndex], values[right]);
		int storeIndex = left;
		for (int i = left; i < right; ++i) {
			if (values[i] < pivotValue) {
				std::swap(values[i], values[storeIndex]);
				++storeIndex;
			}
		}
		std::swap(values[storeIndex], values[right]);
		return storeIndex;
	}

	void sortAll() {
		if (!values.empty()) {
			quickSort(0, static_cast<int>(values.size()) - 1);
		}
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
	void quickSort(int left, int right) {
		if (left >= right) {
			return;
		}

		const int pivotIndex = medianOfThree(left, right);
		const int newPivot = partition(left, right, pivotIndex);
		quickSort(left, newPivot - 1);
		quickSort(newPivot + 1, right);
	}

	std::vector<int> values;
};

int main() {
	QuickSortToolkit toolkit;
	for (int value : { 7, 2, 9, 4, 1, 8 }) {
		toolkit.add(value);
	}

	toolkit.sortAll();
	std::cout << "Sorted values: " << toolkit.toString() << "\n";
}

