#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>
#include "maze-under-test.hpp"

using Cells = std::array<int, 125>;
using Position = std::array<int, 3>;

std::string encode(const Cells& cells) {
    std::ostringstream out;
    for (int cell : cells) out << cell << ' ';
    return out.str();
}

bool load(Pathfinder& maze, const std::string& text) {
    std::istringstream input(text);
    return maze.importMaze(input);
}

std::size_t slot(const Position& p) {
    assert(p[0] >= 0 && p[0] < 5 && p[1] >= 0 && p[1] < 5 && p[2] >= 0 && p[2] < 5);
    return static_cast<std::size_t>(p[0] + 5 * p[1] + 25 * p[2]);
}

bool reachable(const Cells& cells) {
    if (cells.front() == 0 || cells.back() == 0) return false;
    std::array<bool, 125> seen{};
    std::queue<Position> pending;
    pending.push({0, 0, 0});
    seen[0] = true;
    while (!pending.empty()) {
        const Position p = pending.front();
        pending.pop();
        if (p == Position{4, 4, 4}) return true;
        for (int axis = 0; axis < 3; ++axis) {
            for (int delta : {-1, 1}) {
                Position next = p;
                next[axis] += delta;
                if (next[axis] < 0 || next[axis] >= 5) continue;
                const auto index = slot(next);
                if (cells[index] == 1 && !seen[index]) {
                    seen[index] = true;
                    pending.push(next);
                }
            }
        }
    }
    return false;
}

void checkLayout(const Cells& cells) {
    Pathfinder maze;
    assert(load(maze, encode(cells)));
    const auto before = maze.toString();
    const auto path = maze.solveMaze();
#if MAZE_REFERENCE
    assert(!path.empty() == reachable(cells));
    if (!path.empty()) {
        assert((path.front() == Position{0, 0, 0}));
        assert((path.back() == Position{4, 4, 4}));
        std::array<bool, 125> seen{};
        for (std::size_t i = 0; i < path.size(); ++i) {
            const auto index = slot(path[i]);
            assert(cells[index] == 1 && !seen[index]);
            seen[index] = true;
            if (i != 0) {
                int distance = 0;
                for (int axis = 0; axis < 3; ++axis)
                    distance += std::abs(path[i][axis] - path[i - 1][axis]);
                assert(distance == 1);
            }
        }
    }
#else
    assert(path.empty());
#endif
    assert(maze.toString() == before);
    assert(maze.solveMaze() == path);
}

int main() {
    Cells open{};
    open.fill(1);
    Cells blocked{};
    const std::string zeros = encode(blocked);
    const std::vector<std::string> rejected{
        "", " \n\t", "unexpected", "1", zeros.substr(0, zeros.size() - 2),
        zeros + "0", zeros + "unexpected", zeros + "1.5", zeros + "0.0",
        zeros + "999999999999999999999", zeros.substr(0, zeros.size() - 2) + "1.5",
        zeros.substr(0, zeros.size() - 2) + "-2",
        zeros.substr(0, zeros.size() - 2) + "999999999999999999999",
        "2 " + zeros.substr(2), "-1 " + zeros.substr(2),
        "0.5 " + zeros.substr(2), "unexpected " + zeros.substr(2)
    };
    Pathfinder maze;
    assert(load(maze, encode(open)));
    const auto before = maze.toString();
    const auto originalPath = maze.solveMaze();
    for (const auto& input : rejected) {
        assert(!load(maze, input));
        assert(maze.toString() == before);
        assert(maze.solveMaze() == originalPath);
    }
    for (const std::string& text : {encode(open), " \n\t" + encode(blocked) + " \r\n", encode(open).substr(0, 249)})
        assert(load(maze, text));
    std::istringstream failed(encode(blocked));
    failed.setstate(std::ios::badbit);
    const auto previous = maze.toString();
    assert(!maze.importMaze(failed) && maze.toString() == previous);

    checkLayout(open);
    checkLayout(blocked);
    Cells disconnected = open;
    for (int y = 0; y < 5; ++y)
        for (int z = 0; z < 5; ++z) disconnected[slot({2, y, z})] = 0;
    checkLayout(disconnected);
    Cells entranceBlocked = open;
    entranceBlocked.front() = 0;
    checkLayout(entranceBlocked);
    Cells exitBlocked = open;
    exitBlocked.back() = 0;
    checkLayout(exitBlocked);
    for (unsigned seed = 1; seed <= 32; ++seed) {
        Cells cells{};
        unsigned state = seed;
        for (auto& cell : cells) {
            state = state * 1664525U + 1013904223U;
            cell = ((state >> 16U) % 4U) != 0U;
        }
        cells.front() = cells.back() = 1;
        checkLayout(cells);
    }
    std::cout << "17 malformed reloads, stream failure and 37 layouts passed\n";
}
