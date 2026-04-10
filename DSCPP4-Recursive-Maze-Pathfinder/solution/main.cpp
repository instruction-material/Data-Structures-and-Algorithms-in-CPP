#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Pathfinder {
public:
	bool importMaze(std::istream& input) {
		std::vector<int> next;
		int cell = 0;
		while (input >> cell) {
			if (cell != 0 && cell != 1) {
				return false;
			}
			next.push_back(cell);
		}
		if (next.size() != maze.size()) {
			return false;
		}
		std::copy(next.begin(), next.end(), maze.begin());
		return true;
	}

	std::string toString() const {
		std::ostringstream out;
		for (int z = 0; z < 5; ++z) {
			out << "Layer " << z << "\n";
			for (int y = 0; y < 5; ++y) {
				for (int x = 0; x < 5; ++x) {
					out << maze[index(x, y, z)] << ' ';
				}
				out << "\n";
			}
		}
		return out.str();
	}

	std::vector<std::array<int, 3>> solveMaze() {
		std::array<bool, 125> visited{};
		std::vector<std::array<int, 3>> path;
		if (solveFrom(0, 0, 0, visited, path)) {
			return path;
		}
		return {};
	}

private:
	static constexpr std::size_t index(int x, int y, int z) {
		return static_cast<std::size_t>(x + (5 * y) + (25 * z));
	}

	bool inBounds(int x, int y, int z) const {
		return x >= 0 && y >= 0 && z >= 0 && x < 5 && y < 5 && z < 5;
	}

	bool solveFrom(
		int x,
		int y,
		int z,
		std::array<bool, 125>& visited,
		std::vector<std::array<int, 3>>& path
	) {
		if (!inBounds(x, y, z)) {
			return false;
		}

		const std::size_t slot = index(x, y, z);
		if (maze[slot] == 0 || visited[slot]) {
			return false;
		}

		visited[slot] = true;
		path.push_back({ x, y, z });
		if (x == 4 && y == 4 && z == 4) {
			return true;
		}

		static constexpr std::array<std::array<int, 3>, 6> moves{ {
			{ 1, 0, 0 },
			{ -1, 0, 0 },
			{ 0, 1, 0 },
			{ 0, -1, 0 },
			{ 0, 0, 1 },
			{ 0, 0, -1 }
		} };

		for (const auto& move : moves) {
			if (solveFrom(x + move[0], y + move[1], z + move[2], visited, path)) {
				return true;
			}
		}

		path.pop_back();
		return false;
	}

	std::array<int, 125> maze{};
};

int main() {
	std::ostringstream sample;
	for (int z = 0; z < 5; ++z) {
		for (int y = 0; y < 5; ++y) {
			for (int x = 0; x < 5; ++x) {
				const bool open = (x == 0) || (y == 4) || (z == 4);
				sample << (open ? 1 : 0) << ' ';
			}
		}
	}

	Pathfinder pathfinder;
	std::istringstream input(sample.str());
	if (!pathfinder.importMaze(input)) {
		std::cerr << "Failed to import maze\n";
		return 1;
	}

	const auto path = pathfinder.solveMaze();
	std::cout << "Path size: " << path.size() << "\n";
	if (!path.empty()) {
		std::cout << "End coordinate: ("
			<< path.back()[0] << ", "
			<< path.back()[1] << ", "
			<< path.back()[2] << ")\n";
	}
}
