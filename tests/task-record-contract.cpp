#include <cstddef>
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include "task-under-test.hpp"

struct ExpectedTask {
    std::string date;
    std::string description;
    bool done;
};

void require(bool condition) {
    if (!condition) throw std::runtime_error("Task record contract mismatch");
}

void verify(const TodoList& list, const std::vector<ExpectedTask>& model) {
    for (const std::string date : {"2026-05-01", "2026-05-02", "missing"}) {
        std::vector<ExpectedTask> expected;
        for (const auto& task : model) if (task.date == date) expected.push_back(task);
#if TASK_REFERENCE
        std::stable_sort(expected.begin(), expected.end(), [](const auto& a, const auto& b) {
            return std::tie(a.done, a.description) < std::tie(b.done, b.description);
        });
#endif
        const auto actual = list.tasksOnDate(date);
        require(actual.size() == expected.size());
        for (std::size_t i = 0; i < actual.size(); ++i) {
            require(actual[i].dueDate == expected[i].date);
            require(actual[i].description == expected[i].description);
            require(actual[i].completed == expected[i].done);
        }
    }
    auto expected = model;
#if TASK_REFERENCE
    std::stable_sort(expected.begin(), expected.end(), [](const auto& a, const auto& b) {
        return std::tie(a.date, a.done, a.description) < std::tie(b.date, b.done, b.description);
    });
#endif
    std::ostringstream output, wanted;
    wanted << "Task List\n---------\n";
    for (const auto& task : expected)
        wanted << '[' << (task.done ? 'x' : ' ') << "] " << task.date << " - " << task.description << '\n';
    list.printAll(output);
    require(output.str() == wanted.str());
}

int main() {
    TodoList list;
    std::vector<ExpectedTask> model;
    verify(list, model);
    auto add = [&](const std::string& date, const std::string& description) {
        list.add(date, description);
        model.push_back({date, description, false});
        verify(list, model);
    };
    auto done = [&](const std::string& description) {
        list.markDone(description);
        for (auto& task : model) if (task.description == description) {
            task.done = true;
            break;
        }
        verify(list, model);
    };
    auto remove = [&](const std::string& description) {
        bool expected = false;
        for (std::size_t i = 0; i < model.size(); ++i) if (model[i].description == description) {
            model.erase(model.begin() + static_cast<std::ptrdiff_t>(i));
            expected = true;
            break;
        }
        require(list.remove(description) == expected);
        verify(list, model);
    };
    remove("missing");
    done("missing");
    add("2026-05-02", "Repeat");
    add("2026-05-01", "Repeat");
    add("2026-05-01", "Alpha");
    done("Repeat");
    remove("Repeat"); // Printing a sorted copy must not change first-match storage order.
    require(list.tasksOnDate("2026-05-02").empty());
    require(list.tasksOnDate("2026-05-01").size() == 2);
    done("Repeat");
    remove("Repeat");
    remove("Repeat");
    remove("Alpha");
    const std::string descriptions[] = {"Alpha", "Beta", "Repeat", "missing"};
    std::uint32_t state = 12345;
    for (unsigned i = 0; i < 512; ++i) {
        state = state * 1664525u + 1013904223u;
        const auto& description = descriptions[(state >> 8) % 4];
        switch (state % 3) {
            case 0: add(state & 1u ? "2026-05-01" : "2026-05-02", description); break;
            case 1: done(description); break;
            default: remove(description); break;
        }
    }
    std::cout << "First-match records and 512 state transitions passed\n";
}
