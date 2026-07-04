#include <iostream>
#include <vector>

/*****************
*   CONSTANTS   *
*****************/

constexpr int VALUE_MULTIPLIER = 2;
constexpr int VALUE_OFFSET = 1;
const std::vector<int> SAMPLE_VALUES {3, 8, 13, 21};


/*****************
*   FUNCTIONS   *
*****************/

/**
 * @brief Transform each input value with the lesson formula
 *
 * @param values Values to transform
 *
 * @return Transformed values in the original order
 */
std::vector<int> transform_values(const std::vector<int>& values) {
	std::vector<int> transformed_values;
	transformed_values.reserve(values.size());

	// Transform each value independently so the output order matches the input order
	for (int value : values) {
		transformed_values.push_back(value * VALUE_MULTIPLIER + VALUE_OFFSET);
	}

	return transformed_values;
}

/**
 * @brief Print the transformed sample values
 *
 * @return Program exit status
 */
int main() {
	// Print each transformed value on its own line
	for (int value : transform_values(SAMPLE_VALUES)) {
		std::cout << value << "\n";
	}

	return 0;
}
