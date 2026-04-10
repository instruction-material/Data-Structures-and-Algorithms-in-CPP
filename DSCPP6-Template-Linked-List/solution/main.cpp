#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

template <typename T>
class SinglyLinkedList {
public:
	~SinglyLinkedList() {
		clear();
	}

	void insertHead(const T& value) {
		head = new Node{ value, head };
		++count;
	}

	void insertTail(const T& value) {
		if (head == nullptr) {
			insertHead(value);
			return;
		}

		Node* current = head;
		while (current->next != nullptr) {
			current = current->next;
		}
		current->next = new Node{ value, nullptr };
		++count;
	}

	void insertAfter(const T& value, const T& afterValue) {
		Node* current = head;
		while (current != nullptr) {
			if (current->value == afterValue) {
				current->next = new Node{ value, current->next };
				++count;
				return;
			}
			current = current->next;
		}
	}

	bool remove(const T& value) {
		Node* current = head;
		Node* previous = nullptr;
		while (current != nullptr) {
			if (current->value == value) {
				if (previous == nullptr) {
					head = current->next;
				}
				else {
					previous->next = current->next;
				}
				delete current;
				--count;
				return true;
			}
			previous = current;
			current = current->next;
		}
		return false;
	}

	void clear() {
		while (head != nullptr) {
			Node* doomed = head;
			head = head->next;
			delete doomed;
		}
		count = 0;
	}

	int size() const {
		return count;
	}

	const T& at(int index) const {
		if (index < 0 || index >= count) {
			throw std::out_of_range("index");
		}
		Node* current = head;
		for (int step = 0; step < index; ++step) {
			current = current->next;
		}
		return current->value;
	}

	std::string toString() const {
		std::ostringstream out;
		Node* current = head;
		while (current != nullptr) {
			out << current->value;
			if (current->next != nullptr) {
				out << " -> ";
			}
			current = current->next;
		}
		return out.str();
	}

private:
	struct Node {
		T value;
		Node* next;
	};

	Node* head = nullptr;
	int count = 0;
};

int main() {
	SinglyLinkedList<std::string> list;
	list.insertHead("AVL");
	list.insertTail("BST");
	list.insertAfter("Graphs", "AVL");
	list.remove("BST");
	list.insertTail("Recursion");

	std::cout << "List: " << list.toString() << "\n";
	std::cout << "Element 1: " << list.at(1) << "\n";
	std::cout << "Size: " << list.size() << "\n";
}
