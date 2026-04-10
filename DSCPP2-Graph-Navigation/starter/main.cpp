#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

class GraphNavigator {
public:
	bool readNetwork(std::istream& input) {
		int nodeCount = 0;
		if (!(input >> nodeCount) || nodeCount <= 0) {
			return false;
		}

		matrix.assign(nodeCount, std::vector<int>(nodeCount, -1));
		for (int row = 0; row < nodeCount; ++row) {
			for (int col = 0; col < nodeCount; ++col) {
				if (!(input >> matrix[row][col])) {
					matrix.clear();
					return false;
				}
			}
		}
		return true;
	}

	std::vector<int> shortestPath(int start, int goal) const {
		if (matrix.empty() || start < 0 || goal < 0
			|| start >= static_cast<int>(matrix.size())
			|| goal >= static_cast<int>(matrix.size())) {
			return {};
		}

		const int infinity = std::numeric_limits<int>::max() / 4;
		std::vector<int> distance(matrix.size(), infinity);
		std::vector<int> parent(matrix.size(), -1);
		std::vector<bool> visited(matrix.size(), false);
		distance[start] = 0;

		// TODO: replace this passable baseline with a cleaner priority-queue Dijkstra.
		for (std::size_t step = 0; step < matrix.size(); ++step) {
			int next = -1;
			for (std::size_t i = 0; i < matrix.size(); ++i) {
				if (!visited[i] && (next == -1 || distance[i] < distance[next])) {
					next = static_cast<int>(i);
				}
			}
			if (next == -1 || distance[next] == infinity) {
				break;
			}
			visited[next] = true;

			for (std::size_t neighbor = 0; neighbor < matrix.size(); ++neighbor) {
				const int weight = matrix[next][neighbor];
				if (weight < 0) {
					continue;
				}
				if (distance[next] + weight < distance[neighbor]) {
					distance[neighbor] = distance[next] + weight;
					parent[neighbor] = next;
				}
			}
		}

		if (distance[goal] == infinity) {
			return {};
		}

		std::vector<int> path;
		for (int current = goal; current != -1; current = parent[current]) {
			path.push_back(current);
		}
		std::reverse(path.begin(), path.end());
		return path;
	}

private:
	std::vector<std::vector<int>> matrix;
};

int main() {
	std::istringstream sample(
		"5\n"
		"-1 7 3 -1 -1\n"
		"7 -1 1 4 -1\n"
		"3 1 -1 2 8\n"
		"-1 4 2 -1 2\n"
		"-1 -1 8 2 -1\n"
	);

	GraphNavigator navigator;
	if (!navigator.readNetwork(sample)) {
		std::cerr << "Failed to read network\n";
		return 1;
	}

	const auto path = navigator.shortestPath(0, 4);
	std::cout << "Starter path:";
	for (int node : path) {
		std::cout << ' ' << node;
	}
	std::cout << "\n";
}

