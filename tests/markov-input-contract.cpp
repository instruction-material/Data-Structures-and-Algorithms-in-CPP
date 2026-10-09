#include <cassert>
#include <clocale>
#include <limits>
#include <stdexcept>
#include "markov-under-test.hpp"

#if MARKOV_REFERENCE
using Model = std::map<State, std::vector<std::string>>;

// Construct expected windows by corpus position, independently of the deque
// update used in the supplied reference.
Model expectedWindows(const std::vector<std::string>& tokens, std::size_t order) {
    Model expected;
    for (std::size_t position = 0; position < tokens.size(); ++position) {
        State window;
        for (std::size_t offset = order; offset > 0; --offset) {
            window.push_back(position < offset ? "" : tokens[position - offset]);
        }
        expected[window].push_back(tokens[position]);
    }
    return expected;
}

template<class Operation> void rejectsNegative(Operation operation) {
    bool rejected = false;
    try { operation(); }
    catch (const std::invalid_argument&) { rejected = true; }
    assert(rejected);
}

void verifyGeneratedPath(const Model& expected, std::size_t order,
                         int requested, const std::vector<std::string>& output) {
    assert(output.size() <= static_cast<std::size_t>(requested));
    for (std::size_t position = 0; position <= output.size(); ++position) {
        State window;
        for (std::size_t offset = order; offset > 0; --offset) {
            window.push_back(position < offset ? "" : output[position - offset]);
        }
        const auto choices = expected.find(window);
        if (position == output.size()) {
            assert(output.size() == static_cast<std::size_t>(requested) ||
                   choices == expected.end() || choices->second.empty());
        } else {
            assert(choices != expected.end());
            bool observed = false;
            for (const auto& token : choices->second) observed |= token == output[position];
            assert(observed);
        }
    }
}
#endif

int main() {
    assert(std::setlocale(LC_CTYPE, "C"));
    assert(cleanToken("A-b_C!23") == "abc");
    assert(cleanToken("?!123") == "");
    assert(cleanToken(std::string("a") + '\xFF' + "B") == "ab");
    assert(tokenize("").empty());
    assert(tokenize("?! 123").empty());
    assert((tokenize("Alpha ALPHA beta, a-b") == std::vector<std::string>{"alpha", "alpha", "beta", "ab"}));

#if MARKOV_REFERENCE
    for (int negative : {-1, std::numeric_limits<int>::min()}) {
        rejectsNegative([&] { (void)buildModel({"alpha"}, negative); });
        rejectsNegative([&] { (void)generateText({}, negative, 0); });
        rejectsNegative([&] { (void)generateText(buildModel({"alpha"}, 0), 0, negative); });
    }
    assert((buildModel({"alpha", "beta", "alpha"}, 0).at(State{}) ==
            std::vector<std::string>{"alpha", "beta", "alpha"}));
    assert(generateText(buildModel({"alpha"}, 4), 4, 18) == std::vector<std::string>{"alpha"});
    Model emptySuccessors;
    emptySuccessors[State{}] = {};
    assert(generateText(emptySuccessors, 0, 18).empty());

    const std::vector<std::string> vocabulary{"alpha", "beta", "gamma"};
    unsigned sequence = 719;
    for (std::size_t fixture = 0; fixture < 64; ++fixture) {
        std::vector<std::string> corpus;
        for (std::size_t index = 0; index < fixture % 7; ++index) {
            sequence = sequence * 1664525U + 1013904223U;
            corpus.push_back(vocabulary[sequence % vocabulary.size()]);
        }
        for (std::size_t order = 0; order <= 4; ++order) {
            const auto expected = expectedWindows(corpus, order);
            const int parameter = static_cast<int>(order);
            const auto actual = buildModel(corpus, parameter);
            assert(actual == expected);
            for (int length : {0, 1, 2, 18}) {
                const auto output = generateText(actual, parameter, length);
                assert(output == generateText(actual, parameter, length));
                verifyGeneratedPath(expected, order, length, output);
                assert(actual == expected);
            }
        }
    }
    std::cout << "Token cleanup, 320 window models and input guards passed\n";
#else
    std::cout << "Token cleanup and unfinished preview passed\n";
#endif
}
