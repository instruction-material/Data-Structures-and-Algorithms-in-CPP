#include <cctype>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

std::string cleanToken(const std::string& raw) {
	std::string cleaned;
	for (unsigned char ch : raw) {
		if (std::isalpha(ch)) {
			cleaned.push_back(static_cast<char>(std::tolower(ch)));
		}
	}
	return cleaned;
}

std::vector<std::string> tokenize(const std::string& text) {
	std::istringstream input(text);
	std::vector<std::string> tokens;
	std::string raw;
	while (input >> raw) {
		const std::string cleaned = cleanToken(raw);
		if (!cleaned.empty()) {
			tokens.push_back(cleaned);
		}
	}
	return tokens;
}

int main() {
	const std::string sample =
		"Structures matter. Trees matter. Lists matter when the data changes shape.";

	const auto tokens = tokenize(sample);
	std::set<std::string> unique(tokens.begin(), tokens.end());

	std::cout << "Token count: " << tokens.size() << "\n";
	std::cout << "Unique count: " << unique.size() << "\n";
	std::cout << "Starter preview:";
	for (std::size_t i = 0; i < tokens.size() && i < 6; ++i) {
		std::cout << ' ' << tokens[i];
	}
	std::cout << "\n";
	std::cout << "TODO: replace the preview with a state-based text generator.\n";
}

