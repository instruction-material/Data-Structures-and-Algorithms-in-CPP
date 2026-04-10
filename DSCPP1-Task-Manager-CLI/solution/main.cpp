#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

struct Task {
	std::string dueDate;
	std::string description;
	bool completed = false;
};

class TodoList {
public:
	void add(std::string dueDate, std::string description) {
		tasks.push_back({ std::move(dueDate), std::move(description), false });
	}

	bool remove(const std::string& description) {
		const auto before = tasks.size();
		std::erase_if(tasks, [&](const Task& task) {
			return task.description == description;
		});
		return before != tasks.size();
	}

	void markDone(const std::string& description) {
		for (auto& task : tasks) {
			if (task.description == description) {
				task.completed = true;
				return;
			}
		}
	}

	std::vector<Task> tasksOnDate(const std::string& dueDate) const {
		std::vector<Task> result;
		for (const auto& task : tasks) {
			if (task.dueDate == dueDate) {
				result.push_back(task);
			}
		}

		std::ranges::sort(result, [](const Task& left, const Task& right) {
			if (left.completed != right.completed) {
				return left.completed < right.completed;
			}
			return left.description < right.description;
		});
		return result;
	}

	void printAll(std::ostream& out) const {
		std::vector<Task> ordered = tasks;
		std::ranges::sort(ordered, [](const Task& left, const Task& right) {
			if (left.dueDate != right.dueDate) {
				return left.dueDate < right.dueDate;
			}
			if (left.completed != right.completed) {
				return left.completed < right.completed;
			}
			return left.description < right.description;
		});

		out << "Task List\n";
		out << "---------\n";
		for (const auto& task : ordered) {
			out << "[" << (task.completed ? 'x' : ' ') << "] "
				<< task.dueDate << " - " << task.description << "\n";
		}
	}

private:
	std::vector<Task> tasks;
};

int main() {
	TodoList list;
	list.add("2026-05-01", "Draft graph notes");
	list.add("2026-05-01", "Practice vector review");
	list.add("2026-05-03", "Read quicksort walkthrough");
	list.markDone("Draft graph notes");
	list.remove("Practice vector review");

	list.printAll(std::cout);
	std::cout << "\nTasks on 2026-05-01:\n";
	for (const auto& task : list.tasksOnDate("2026-05-01")) {
		std::cout << "- " << task.description << "\n";
	}
}

