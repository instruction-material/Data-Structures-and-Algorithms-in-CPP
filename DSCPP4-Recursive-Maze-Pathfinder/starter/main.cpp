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
		// TODO: implement recursive backtracking from (0, 0, 0) to (4, 4, 4).
		return {};
	}

private:
	static constexpr std::size_t index(int x, int y, int z) {
		return static_cast<std::size_t>(x + (5 * y) + (25 * z));
	}

	std::array<int, 125> maze{};
};

int main() {
	std::ostringstream sample;
	for (int i = 0; i < 125; ++i) {
		sample << 1 << ' ';
	}

	Pathfinder pathfinder;
	std::istringstream input(sample.str());
	if (!pathfinder.importMaze(input)) {
		std::cerr << "Failed to import maze\n";
		return 1;
	}

	std::cout << pathfinder.toString();
	std::cout << "Starter path length: " << pathfinder.solveMaze().size() << "\n";
}
