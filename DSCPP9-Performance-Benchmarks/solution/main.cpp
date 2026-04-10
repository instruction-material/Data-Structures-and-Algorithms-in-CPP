#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <set>
#include <unordered_set>
#include <vector>

template <typename Callback>
long long timeRun(Callback callback) {
	const auto start = std::chrono::steady_clock::now();
	callback();
	const auto end = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

class LinkedListSet {
public:
	~LinkedListSet() {
		clear();
	}

	void add(int value) {
		if (contains(value)) {
			return;
		}
		head = new Node{ value, head };
	}

	bool contains(int value) const {
		for (Node* current = head; current != nullptr; current = current->next) {
			if (current->value == value) {
				return true;
			}
		}
		return false;
	}

	void clear() {
		while (head != nullptr) {
			Node* doomed = head;
			head = head->next;
			delete doomed;
		}
	}

private:
	struct Node {
		int value;
		Node* next;
	};

	Node* head = nullptr;
};

class BinarySearchTreeSet {
public:
	~BinarySearchTreeSet() {
		clear(root);
	}

	void add(int value) {
		add(root, value);
	}

	bool contains(int value) const {
		Node* current = root;
		while (current != nullptr) {
			if (value == current->value) {
				return true;
			}
			current = value < current->value ? current->left : current->right;
		}
		return false;
	}

private:
	struct Node {
		int value;
		Node* left = nullptr;
		Node* right = nullptr;
	};

	static void add(Node*& node, int value) {
		if (node == nullptr) {
			node = new Node{ value };
			return;
		}
		if (value == node->value) {
			return;
		}
		if (value < node->value) {
			add(node->left, value);
		}
		else {
			add(node->right, value);
		}
	}

	static void clear(Node* node) {
		if (node == nullptr) {
			return;
		}
		clear(node->left);
		clear(node->right);
		delete node;
	}

	Node* root = nullptr;
};

class AvlSet {
public:
	~AvlSet() {
		clear(root);
	}

	void add(int value) {
		root = insert(root, value);
	}

	bool contains(int value) const {
		Node* current = root;
		while (current != nullptr) {
			if (value == current->value) {
				return true;
			}
			current = value < current->value ? current->left : current->right;
		}
		return false;
	}

private:
	struct Node {
		int value;
		int height = 1;
		Node* left = nullptr;
		Node* right = nullptr;
	};

	static int height(Node* node) {
		return node == nullptr ? 0 : node->height;
	}

	static void refresh(Node* node) {
		node->height = 1 + std::max(height(node->left), height(node->right));
	}

	static int balance(Node* node) {
		return node == nullptr ? 0 : height(node->left) - height(node->right);
	}

	static Node* rotateLeft(Node* x) {
		Node* y = x->right;
		Node* transfer = y->left;
		y->left = x;
		x->right = transfer;
		refresh(x);
		refresh(y);
		return y;
	}

	static Node* rotateRight(Node* y) {
		Node* x = y->left;
		Node* transfer = x->right;
		x->right = y;
		y->left = transfer;
		refresh(y);
		refresh(x);
		return x;
	}

	static Node* rebalance(Node* node) {
		refresh(node);
		const int nodeBalance = balance(node);
		if (nodeBalance > 1) {
			if (balance(node->left) < 0) {
				node->left = rotateLeft(node->left);
			}
			return rotateRight(node);
		}
		if (nodeBalance < -1) {
			if (balance(node->right) > 0) {
				node->right = rotateRight(node->right);
			}
			return rotateLeft(node);
		}
		return node;
	}

	static Node* insert(Node* node, int value) {
		if (node == nullptr) {
			return new Node{ value };
		}
		if (value == node->value) {
			return node;
		}
		if (value < node->value) {
			node->left = insert(node->left, value);
		}
		else {
			node->right = insert(node->right, value);
		}
		return rebalance(node);
	}

	static void clear(Node* node) {
		if (node == nullptr) {
			return;
		}
		clear(node->left);
		clear(node->right);
		delete node;
	}

	Node* root = nullptr;
};

int main() {
	std::mt19937 rng(42);
	std::uniform_int_distribution<int> distribution(1, 100000);
	std::vector<int> values(3000);
	for (int& value : values) {
		value = distribution(rng);
	}

	const auto vectorInsert = timeRun([&] {
		std::vector<int> data;
		for (int value : values) {
			data.push_back(value);
		}
	});

	const auto setInsert = timeRun([&] {
		std::set<int> data;
		for (int value : values) {
			data.insert(value);
		}
	});

	const auto unorderedInsert = timeRun([&] {
		std::unordered_set<int> data;
		for (int value : values) {
			data.insert(value);
		}
	});

	const auto linkedListInsert = timeRun([&] {
		LinkedListSet data;
		for (int value : values) {
			data.add(value);
		}
	});

	const auto bstInsert = timeRun([&] {
		BinarySearchTreeSet data;
		for (int value : values) {
			data.add(value);
		}
	});

	const auto avlInsert = timeRun([&] {
		AvlSet data;
		for (int value : values) {
			data.add(value);
		}
	});

	std::cout << "vector insert (us): " << vectorInsert << "\n";
	std::cout << "set insert (us): " << setInsert << "\n";
	std::cout << "unordered_set insert (us): " << unorderedInsert << "\n";
	std::cout << "linked list insert (us): " << linkedListInsert << "\n";
	std::cout << "bst insert (us): " << bstInsert << "\n";
	std::cout << "avl insert (us): " << avlInsert << "\n";
}
