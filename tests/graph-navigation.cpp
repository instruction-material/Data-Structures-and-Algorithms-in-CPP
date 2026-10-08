#include <cstdint>
#include <iostream>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define main courseSampleMain
#include COURSE_SOURCE
#undef main

using namespace std::string_literals;

using Matrix = std::vector<std::vector<int>>;
using Cost = std::int64_t;
constexpr Cost infinity = std::numeric_limits<Cost>::max();

void require(bool condition, const char* explanation) {
    if (!condition) throw std::runtime_error(explanation);
}

std::string encode(const Matrix& matrix) {
    std::ostringstream out;
    out << matrix.size() << '\n';
    for (const auto& row : matrix) {
        for (int weight : row) out << weight << ' ';
        out << '\n';
    }
    return out.str();
}

void load(GraphNavigator& graph, const Matrix& matrix) {
    std::istringstream in(encode(matrix));
    require(graph.readNetwork(in), "valid graph rejected");
}

std::vector<std::vector<Cost>> oracle(const Matrix& matrix) {
    const auto count = matrix.size();
    std::vector<std::vector<Cost>> result(count, std::vector<Cost>(count, infinity));
    for (std::size_t i = 0; i < count; ++i) {
        result[i][i] = 0;
        for (std::size_t j = 0; j < count; ++j) {
            if (matrix[i][j] >= 0 && matrix[i][j] < result[i][j])
                result[i][j] = matrix[i][j];
        }
    }
    for (std::size_t k = 0; k < count; ++k)
        for (std::size_t i = 0; i < count; ++i)
            for (std::size_t j = 0; j < count; ++j)
                if (result[i][k] != infinity && result[k][j] != infinity &&
                    result[i][k] + result[k][j] < result[i][j])
                    result[i][j] = result[i][k] + result[k][j];
    return result;
}

void checkRoute(const GraphNavigator& graph, const Matrix& matrix,
                int start, int goal, Cost expected) {
    const auto path = graph.shortestPath(start, goal);
    if (expected == infinity) {
        require(path.empty(), "unreachable goal returned a path");
        return;
    }
    require(!path.empty() && path.front() == start && path.back() == goal,
            "route endpoints mismatch");
    require(path.size() <= matrix.size(), "route is not simple");
    std::set<int> visited;
    Cost actual = 0;
    for (std::size_t i = 0; i < path.size(); ++i) {
        const auto node = path[i];
        require(node >= 0 && node < static_cast<int>(matrix.size()), "unknown route node");
        require(visited.insert(node).second, "parent reconstruction contains a cycle");
        if (i) {
            const int edge = matrix[static_cast<std::size_t>(path[i - 1])]
                                   [static_cast<std::size_t>(node)];
            require(edge >= 0, "route crosses an absent edge");
            actual += edge;
        }
    }
    require(actual == expected, "route is not a least-cost path");
    require(graph.costOfPath(path) == actual, "cost helper disagrees with independent sum");
}

void rejectsPath(const GraphNavigator& graph, std::vector<int> path) {
    bool rejected = false;
    try { (void)graph.costOfPath(path); }
    catch (const std::invalid_argument&) { rejected = true; }
    require(rejected, "invalid path was not rejected");
}

int main() {
    try {
        GraphNavigator graph;
        require(graph.shortestPath(0, 0).empty(), "empty graph accepted an endpoint");
        require(graph.costOfPath({}) == 0, "empty path contract changed");
        rejectsPath(graph, {0});
        const Matrix original{{-1, 7}, {-1, -1}};
        load(graph, original);
        const std::vector<std::string> malformed{
            "", "0", "-1", "257", "2147483647", "9223372036854775808",
            "2 -1 7 -1", "2 -1 7 -1 x", "2 -1 -2 -1 -1",
            "2 -1 2147483648 -1 -1", "2 -1 7 -1 -1 extra",
            "2 -1 7 -1 -1 9", "2.5 -1 7 -1 -1", "2 -1 7 -1 -1\0junk"s
        };
        for (const auto& text : malformed) {
            std::istringstream in(text);
            require(!graph.readNetwork(in), "malformed input accepted");
            checkRoute(graph, original, 0, 1, 7);
            require(graph.shortestPath(1, 0).empty(), "rejection changed edge direction");
        }
        std::istringstream bad("2 -1 7 -1 -1");
        bad.setstate(std::ios::badbit);
        require(!graph.readNetwork(bad), "bad stream accepted");
        checkRoute(graph, original, 0, 1, 7);
        require(graph.shortestPath(-1, 1).empty() && graph.shortestPath(0, 2).empty(),
                "invalid endpoints accepted");
        rejectsPath(graph, {-1});
        rejectsPath(graph, {2});
        rejectsPath(graph, {1, 0});
        rejectsPath(graph, {0, 2});
        require(graph.costOfPath({0}) == 0, "valid singleton path failed");

        const int big = std::numeric_limits<int>::max();
        const Matrix large{{-1, big, -1}, {-1, -1, big}, {-1, -1, -1}};
        load(graph, large);
        checkRoute(graph, large, 0, 2, Cost(big) * 2);
        require(graph.shortestPath(2, 0).empty(), "successful reload retained old edges");

        std::uint64_t state = 23901;
        std::size_t routeCases = 0;
        for (std::size_t n = 1; n <= 8; ++n) {
            for (int trial = 0; trial < 4; ++trial) {
                Matrix matrix(n, std::vector<int>(n, -1));
                for (auto& row : matrix) for (auto& edge : row) {
                    state = state * 6364136223846793005ULL + 1;
                    const auto choice = state >> 32;
                    if (choice % 4 == 0) edge = -1;
                    else if (choice % 7 == 0) edge = big;
                    else edge = static_cast<int>(choice % 32);
                }
                load(graph, matrix);
                const auto expected = oracle(matrix);
                for (std::size_t start = 0; start < n; ++start)
                    for (std::size_t goal = 0; goal < n; ++goal) {
                        checkRoute(graph, matrix, static_cast<int>(start),
                                   static_cast<int>(goal), expected[start][goal]);
                        ++routeCases;
                    }
            }
        }
        require(routeCases == 816, "oracle coverage count changed");

        Matrix deepest(GraphNavigator::maxNodes,
                       std::vector<int>(GraphNavigator::maxNodes, -1));
        for (std::size_t i = 1; i < deepest.size(); ++i) deepest[i - 1][i] = big;
        load(graph, deepest);
        checkRoute(graph, deepest, 0, static_cast<int>(deepest.size() - 1),
                   Cost(big) * static_cast<Cost>(deepest.size() - 1));
        require(graph.shortestPath(255, 0).empty(), "maximum graph is incorrectly undirected");

        const Matrix zero{{0, 0, big}, {0, -1, 0}, {-1, -1, -1}};
        load(graph, zero);
        checkRoute(graph, zero, 0, 2, 0);
        checkRoute(graph, zero, 1, 1, 0);
        checkRoute(graph, zero, 2, 0, infinity);
        std::cout << "Graph regressions passed: 14 malformed loads, bad stream, "
                     "32 independent graphs, 816 endpoint comparisons, large costs, "
                     "256-node chain and zero-cost cycles\n";
        return 0;
    } catch (const std::exception& failure) {
        std::cerr << "Graph regression failed: " << failure.what() << '\n';
        return 1;
    }
}
