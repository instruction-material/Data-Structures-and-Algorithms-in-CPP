#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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
        std::vector<bool> visited(matrix.size(), false);
        distance[static_cast<std::size_t>(start)] = 0;

        // BEGIN TASK: replace the supplied O(V^2) selection with a priority queue.
        // Keep the input contract, parent reconstruction and 64-bit costs intact.
        for (std::size_t step = 0; step < matrix.size(); ++step) {
            int next = -1;
            for (std::size_t i = 0; i < matrix.size(); ++i) {
                if (!visited[i] &&
                    (next == -1 || distance[i] < distance[static_cast<std::size_t>(next)])) {
                    next = static_cast<int>(i);
                }
            }
            if (next == -1 || distance[static_cast<std::size_t>(next)] == infinity) {
                break;
            }
            const auto nextIndex = static_cast<std::size_t>(next);
            visited[nextIndex] = true;

            for (std::size_t neighbor = 0; neighbor < matrix.size();
                 ++neighbor) {
                const int weight = matrix[nextIndex][neighbor];
                if (weight < 0 || visited[neighbor]) {
                    continue;
                }
                if (distance[nextIndex] + weight < distance[neighbor]) {
                    distance[neighbor] = distance[nextIndex] + weight;
                    parent[neighbor] = next;
                }
            }
        }

        // END TASK

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
    std::cout << "Starter path:";
    for (int node : path) {
        std::cout << ' ' << node;
    }
    std::cout << "\n";
    return 0;
}
