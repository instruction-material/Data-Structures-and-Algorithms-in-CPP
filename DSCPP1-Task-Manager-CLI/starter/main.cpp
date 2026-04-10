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
		for (auto it = tasks.begin(); it != tasks.end(); ++it) {
			if (it->description == description) {
				tasks.erase(it);
				return true;
			}
		}
		return false;
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

		// TODO: sort by completion status and then alphabetically by description.
		return result;
	}

	void printAll(std::ostream& out) const {
		out << "Task List\n";
		out << "---------\n";
		for (const auto& task : tasks) {
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
	std::cout << "\nTasks on 2026-05-01: " << list.tasksOnDate("2026-05-01").size() << "\n";
}

