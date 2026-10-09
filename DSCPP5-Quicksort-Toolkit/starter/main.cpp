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
        // ADDED: TODO: partition the inclusive range around the pivot value.
        (void)left;
        (void)right;
        return pivotIndex;
    }

    void sortAll() {
        // CHANGED: supplied entry point leaves sorting to the learner tasks.
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
    // ADDED: third learner task; the provided vector owns the storage.
    void quickSort(int left, int right) {
        // TODO: stop at zero/one item, partition, then sort smaller subranges.
        (void)left;
        (void)right;
    }

    std::vector<int> values;
};

int main() {
    QuickSortToolkit toolkit;
    for (int value : {7, 2, 9, 4, 1, 8}) {
        toolkit.add(value);
    }

    toolkit.sortAll();
    std::cout << "Starter values (quicksort pending): " << toolkit.toString() << "\n";
}
