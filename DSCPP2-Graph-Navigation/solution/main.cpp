#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/****************
*   SOLUTION   *
****************/

class GraphNavigator {
  public:
    using Cost = std::int64_t; // CHANGED: distances can exceed int.
    static constexpr std::size_t maxNodes = 256;
    static_assert(std::numeric_limits<Cost>::max() / maxNodes >=
                  std::numeric_limits<int>::max());

    // CHANGED: validate a bounded candidate before replacing the loaded graph.
    bool readNetwork(std::istream& input) {
        long long nodeCount = 0;
        if (!(input >> nodeCount) || nodeCount <= 0 ||
            nodeCount > static_cast<long long>(maxNodes)) {
            return false;
        }

        const auto count = static_cast<std::size_t>(nodeCount);
        std::vector<std::vector<int>> candidate(count, std::vector<int>(count));
        for (auto& row : candidate) {
            for (auto& weight : row) {
                if (!(input >> weight) || weight < -1) {
                    return false;
                }
            }
        }
        input >> std::ws;
        if (input.bad() || !input.eof()) {
            return false;
        }
        matrix.swap(candidate);
        return true;
    }

    std::vector<int> shortestPath(int start, int goal) const {
        if (matrix.empty() || start < 0 || goal < 0 ||
            start >= static_cast<int>(matrix.size()) ||
            goal >= static_cast<int>(matrix.size())) {
            return {};
        }

        const Cost infinity = std::numeric_limits<Cost>::max();
        std::vector<Cost> distance(matrix.size(), infinity);
        std::vector<int> parent(matrix.size(), -1);
        using QueueItem = std::pair<Cost, int>;
        std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<>>
            frontier;

        distance[static_cast<std::size_t>(start)] = 0;
        frontier.push({0, start});

        while (!frontier.empty()) {
            const auto [cost, node] = frontier.top();
            frontier.pop();

            const auto nodeIndex = static_cast<std::size_t>(node);
            if (cost != distance[nodeIndex]) {
                continue;
            }
            if (node == goal) {
                break;
            }

            for (std::size_t neighbor = 0; neighbor < matrix.size();
                 ++neighbor) {
                const int weight = matrix[nodeIndex][neighbor];
                if (weight < 0) {
                    continue;
                }

                const Cost nextCost = cost + weight;
                if (nextCost < distance[neighbor]) {
                    distance[neighbor] = nextCost;
                    parent[neighbor] = node;
                    frontier.push({nextCost, static_cast<int>(neighbor)});
                }
            }
        }

        if (distance[static_cast<std::size_t>(goal)] == infinity) {
            return {};
        }

        std::vector<int> path;
        for (int current = goal; current != -1; current = parent[static_cast<std::size_t>(current)]) {
            path.push_back(current);
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    // CHANGED: validate every path node/edge and keep large costs representable.
    Cost costOfPath(const std::vector<int>& path) const {
        for (int node : path) {
            if (node < 0 || node >= static_cast<int>(matrix.size())) {
                throw std::invalid_argument("Path contains an unknown node.");
            }
        }
        Cost total = 0;
        for (std::size_t i = 1; i < path.size(); ++i) {
            const int weight = matrix[static_cast<std::size_t>(path[i - 1])]
                                     [static_cast<std::size_t>(path[i])];
            if (weight < 0) {
                throw std::invalid_argument("Path crosses an absent edge.");
            }
            if (total > std::numeric_limits<Cost>::max() - weight) {
                throw std::overflow_error("Path cost is too large.");
            }
            total += weight;
        }
        return total;
    }

  private:
    std::vector<std::vector<int>> matrix;
};

int main() {
    std::istringstream sample("5\n"
                              "-1 7 3 -1 -1\n"
                              "7 -1 1 4 -1\n"
                              "3 1 -1 2 8\n"
                              "-1 4 2 -1 2\n"
                              "-1 -1 8 2 -1\n");

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
    return 0;
}
