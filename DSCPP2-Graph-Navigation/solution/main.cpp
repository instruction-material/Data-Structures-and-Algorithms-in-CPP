#include <algorithm>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
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
		using QueueItem = std::pair<int, int>;
		std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<>> frontier;

		distance[start] = 0;
		frontier.push({ 0, start });

		while (!frontier.empty()) {
			const auto [cost, node] = frontier.top();
			frontier.pop();

			if (cost != distance[node]) {
				continue;
			}
			if (node == goal) {
				break;
			}

			for (std::size_t neighbor = 0; neighbor < matrix.size(); ++neighbor) {
				const int weight = matrix[node][neighbor];
				if (weight < 0) {
					continue;
				}

				const int nextCost = cost + weight;
				if (nextCost < distance[neighbor]) {
					distance[neighbor] = nextCost;
					parent[neighbor] = node;
					frontier.push({ nextCost, static_cast<int>(neighbor) });
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

	int costOfPath(const std::vector<int>& path) const {
		if (path.size() < 2) {
			return 0;
		}

		int total = 0;
		for (std::size_t i = 1; i < path.size(); ++i) {
			total += matrix[path[i - 1]][path[i]];
		}
		return total;
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
	std::cout << "Shortest path:";
	for (int node : path) {
		std::cout << ' ' << node;
	}
	std::cout << "\nCost: " << navigator.costOfPath(path) << "\n";
}

