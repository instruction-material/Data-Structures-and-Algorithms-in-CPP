#include <cctype>
#include <deque>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using State = std::deque<std::string>;

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

std::map<State, std::vector<std::string>> buildModel(
	const std::vector<std::string>& tokens,
	int order
) {
	std::map<State, std::vector<std::string>> model;
	State state(order, "");
	for (const auto& token : tokens) {
		model[state].push_back(token);
		state.push_back(token);
		state.pop_front();
	}
	return model;
}

std::vector<std::string> generateText(
	const std::map<State, std::vector<std::string>>& model,
	int order,
	int outputLength
) {
	std::mt19937 rng(42);
	State state(order, "");
	std::vector<std::string> output;

	for (int i = 0; i < outputLength; ++i) {
		const auto found = model.find(state);
		if (found == model.end() || found->second.empty()) {
			break;
		}

		std::uniform_int_distribution<std::size_t> pick(0, found->second.size() - 1);
		const std::string next = found->second[pick(rng)];
		output.push_back(next);
		state.push_back(next);
		state.pop_front();
	}

	return output;
}

int main() {
	const std::string sample =
		"Structures matter. Trees matter. Lists matter when the data changes shape. "
		"Trees organize order. Lists preserve sequence. Hashing changes the lookup story.";

	const auto tokens = tokenize(sample);
	std::set<std::string> unique(tokens.begin(), tokens.end());
	const auto model = buildModel(tokens, 2);
	const auto generated = generateText(model, 2, 18);

	std::cout << "Token count: " << tokens.size() << "\n";
	std::cout << "Unique count: " << unique.size() << "\n";
	std::cout << "Generated:";
	for (const auto& token : generated) {
		std::cout << ' ' << token;
	}
	std::cout << "\n";
}
